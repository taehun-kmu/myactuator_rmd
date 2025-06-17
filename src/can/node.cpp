#include "myactuator_rmd/can/node.hpp"

#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

#include "myactuator_rmd/can/exceptions.hpp"
#include "myactuator_rmd/can/frame.hpp"
#include "myactuator_rmd/can/utilities.hpp"

namespace myactuator_rmd {
namespace can {

// Implementation class definition
class Node::Impl {
 public:
  explicit Impl(std::string const& ifname,
                std::chrono::microseconds const& send_timeout,
                std::chrono::microseconds const& receive_timeout,
                bool const is_signal_errors);
  ~Impl();

  // Delete copy constructor and assignment operator
  Impl(Impl const&) = delete;
  Impl& operator=(Impl const&) = delete;

  // Delete move constructor and assignment operator
  Impl(Impl&&) = delete;
  Impl& operator=(Impl&&) = delete;

  // Communication methods
  [[nodiscard]] Frame read() const;
  void write(Frame const& frame);
  void write(std::uint32_t const can_id,
             std::array<std::uint8_t, 8> const& data);

  // Configuration methods
  void setLoopback(bool const is_loopback);
  void setRecvFilter(std::vector<std::uint32_t> const& can_ids,
                     bool const is_invert = false);
  void setSendTimeout(std::chrono::microseconds const& timeout);
  void setRecvTimeout(std::chrono::microseconds const& timeout);
  void setErrorFilters(bool const is_signal_errors);

 private:
  // Utility methods
  void initSocket(std::string const& ifname);
  void closeSocket() noexcept;

  // Data members
  std::string ifname_;
  int socket_;
};

// Impl class constructor
Node::Impl::Impl(std::string const& ifname,
                  std::chrono::microseconds const& send_timeout,
                  std::chrono::microseconds const& receive_timeout,
                  bool const is_signal_errors)
    : ifname_{}, socket_{-1} {
  initSocket(ifname);
  setSendTimeout(send_timeout);
  setRecvTimeout(receive_timeout);
  setErrorFilters(is_signal_errors);
}

Node::Impl::~Impl() {
  closeSocket();
}

// Node class constructor and destructor
Node::Node(std::string const& ifname,
           std::chrono::microseconds const& send_timeout,
           std::chrono::microseconds const& receive_timeout,
           bool const is_signal_errors)
    : pimpl_{std::make_unique<Impl>(ifname, send_timeout, receive_timeout, is_signal_errors)} {
}

Node::~Node() = default;

// Node public methods - delegate to Impl
void Node::setLoopback(bool const is_loopback) {
  pimpl_->setLoopback(is_loopback);
}

void Node::setRecvFilter(std::vector<std::uint32_t> const& can_ids,
                         bool const is_invert) {
  pimpl_->setRecvFilter(can_ids, is_invert);
}

void Node::setSendTimeout(std::chrono::microseconds const& timeout) {
  pimpl_->setSendTimeout(timeout);
}

void Node::setRecvTimeout(std::chrono::microseconds const& timeout) {
  pimpl_->setRecvTimeout(timeout);
}

void Node::setErrorFilters(bool const is_signal_errors) {
  pimpl_->setErrorFilters(is_signal_errors);
}

Frame Node::read() const {
  return pimpl_->read();
}

void Node::write(Frame const& frame) {
  pimpl_->write(frame);
}

void Node::write(std::uint32_t const can_id,
                 std::array<std::uint8_t, 8> const& data) {
  pimpl_->write(can_id, data);
}

// Impl class method implementations
void Node::Impl::setLoopback(bool const is_loopback) {
  int const recv_own_msgs{static_cast<int>(is_loopback)};
  if (::setsockopt(socket_, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recv_own_msgs,
                   sizeof(int)) < 0) {
    throw SocketException(
        errno, std::generic_category(),
        "Interface '" + ifname_ + "' - Could not configure loopback");
  }
}

void Node::Impl::setRecvFilter(std::vector<std::uint32_t> const& can_ids,
                               bool const is_invert) {
  std::vector<struct ::can_filter> filters{};
  filters.resize(can_ids.size());
  for (std::size_t i = 0; i < can_ids.size(); ++i) {
    auto const& can_id{can_ids[i]};
    if (is_invert) {
      filters[i].can_id = can_id | CAN_INV_FILTER;
    } else {
      filters[i].can_id = can_id;
    }
    filters[i].can_mask = CAN_SFF_MASK;
  }
  if (::setsockopt(socket_, SOL_CAN_RAW, CAN_RAW_FILTER, filters.data(),
                   sizeof(::can_filter) * filters.size()) < 0) {
    throw SocketException(
        errno, std::generic_category(),
        "Interface '" + ifname_ + "' - Could not configure read filter");
  }
}

void Node::Impl::setSendTimeout(std::chrono::microseconds const& timeout) {
  struct ::timeval const send_timeout{myactuator_rmd::toTimeval(timeout)};
  if (::setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO,
                   reinterpret_cast<const char*>(&send_timeout),
                   sizeof(struct ::timeval)) < 0) {
    throw SocketException(
        errno, std::generic_category(),
        "Interface '" + ifname_ + "' - Error setting socket timeout");
  }
}

void Node::Impl::setRecvTimeout(std::chrono::microseconds const& timeout) {
  struct ::timeval const recv_timeout{myactuator_rmd::toTimeval(timeout)};
  if (::setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO,
                   reinterpret_cast<const char*>(&recv_timeout),
                   sizeof(struct ::timeval)) < 0) {
    throw SocketException(
        errno, std::generic_category(),
        "Interface '" + ifname_ + "' - Error setting socket timeout");
  }
}

void Node::Impl::setErrorFilters(bool const is_signal_errors) {
  // See
  // https://github.com/linux-can/can-utils/blob/master/include/linux/can/error.h
  ::can_err_mask_t err_mask{};
  if (is_signal_errors) {
    err_mask = (CAN_ERR_TX_TIMEOUT | CAN_ERR_LOSTARB | CAN_ERR_CRTL |
                CAN_ERR_PROT | CAN_ERR_TRX | CAN_ERR_ACK | CAN_ERR_BUSOFF |
                CAN_ERR_BUSERROR | CAN_ERR_RESTARTED);
  }
  if (::setsockopt(socket_, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &err_mask,
                   sizeof(::can_err_mask_t)) < 0) {
    throw SocketException(
        errno, std::generic_category(),
        "Interface '" + ifname_ + "' - Error setting error acknowledgement");
  }
}

Frame Node::Impl::read() const {
  struct ::can_frame frame {};
  if (::read(socket_, &frame, sizeof(struct ::can_frame)) < 0) {
    throw SocketException(
        errno, std::generic_category(),
        "Interface '" + ifname_ + "' - Could not read CAN frame");
  }
  // We will only receive these frames if the corresponding error mask is set
  // See
  // https://github.com/linux-can/can-utils/blob/master/include/linux/can/error.h
  if (frame.can_id & CAN_ERR_FLAG) {
    std::ostringstream ss{};
    ss << frame;
    if (frame.can_id & CAN_ERR_TX_TIMEOUT) {
      throw TxTimeoutError("Send timeout");
    } else if (frame.can_id & CAN_ERR_LOSTARB) {
      throw LostArbitrationError("CAN frame '" + ss.str() + "'");
    } else if (frame.can_id & CAN_ERR_CRTL) {
      throw ControllerProblemError("CAN frame '" + ss.str() + "'");
    } else if (frame.can_id & CAN_ERR_PROT) {
      throw ProtocolViolationError("CAN frame '" + ss.str() + "'");
    } else if (frame.can_id & CAN_ERR_TRX) {
      throw TransceiverStatusError("CAN frame '" + ss.str() + "'");
    } else if (frame.can_id & CAN_ERR_ACK) {
      throw NoAcknowledgeError("No acknowledgement from receiver");
    } else if (frame.can_id & CAN_ERR_BUSOFF) {
      throw BusOffError("Bus off");
    } else if (frame.can_id & CAN_ERR_BUSERROR) {
      throw BusError("Bus error");
    } else if (frame.can_id & CAN_ERR_RESTARTED) {
      throw ControllerRestartedError("Controller restarted");
    } else {
      throw Exception("Unknown CAN protocol error: CAN frame '" + ss.str() +
                      "'");
    }
  }
  std::array<std::uint8_t, 8> data{};
  std::copy(std::begin(frame.data), std::end(frame.data), std::begin(data));
  Frame const f{frame.can_id, data};
  return f;
}

void Node::Impl::write(Frame const& frame) {
  write(frame.getId(), frame.getData());
}

void Node::Impl::write(std::uint32_t const can_id,
                       std::array<std::uint8_t, 8> const& data) {
  struct ::can_frame frame {};
  frame.can_id = can_id;
  frame.len = 8;
  std::copy(std::begin(data), std::end(data), std::begin(frame.data));
  if (::write(socket_, &frame, sizeof(struct ::can_frame)) !=
      sizeof(struct ::can_frame)) {
    std::ostringstream ss{};
    ss << frame;
    throw SocketException(errno, std::generic_category(),
                          "Interface '" + ifname_ +
                              "' - Could not write CAN frame '" + ss.str() +
                              "'");
  }
}

void Node::Impl::initSocket(std::string const& ifname) {
  ifname_ = ifname;
  socket_ = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (socket_ < 0) {
    throw SocketException(
        errno, std::generic_category(),
        "Interface '" + ifname_ + "' - Error creating socket");
  }

  struct ::ifreq ifr {};
  std::strcpy(ifr.ifr_name, ifname.c_str());
  if (::ioctl(socket_, SIOCGIFINDEX, &ifr) < 0) {
    throw SocketException(
        errno, std::generic_category(),
        "Interface '" + ifname_ + "' - Error manipulating device parameters");
  }

  struct ::sockaddr_can addr {};
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  if (::bind(socket_, reinterpret_cast<struct ::sockaddr*>(&addr),
             sizeof(addr)) < 0) {
    throw SocketException(
        errno, std::generic_category(),
        "Interface '" + ifname_ + "' - Error assigning address to socket");
  }
}

void Node::Impl::closeSocket() noexcept {
  ::close(socket_);
}

}  // namespace can
}  // namespace myactuator_rmd

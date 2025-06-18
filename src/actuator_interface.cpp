#include "myactuator_rmd/actuator_interface.hpp"

#include <chrono>
#include <cstdint>
#include <string>

#include "myactuator_rmd/actuator_state/can_baud_rate.hpp"
#include "myactuator_rmd/actuator_state/control_mode.hpp"
#include "myactuator_rmd/actuator_state/feedback.hpp"
#include "myactuator_rmd/actuator_state/gains.hpp"
#include "myactuator_rmd/actuator_state/motor_status_1.hpp"
#include "myactuator_rmd/actuator_state/motor_status_2.hpp"
#include "myactuator_rmd/actuator_state/motor_status_3.hpp"
#include "myactuator_rmd/debug_checks.hpp"
#include "myactuator_rmd/driver/driver.hpp"
#include "myactuator_rmd/exceptions.hpp"
#include "myactuator_rmd/protocol/requests.hpp"
#include "myactuator_rmd/protocol/responses.hpp"

namespace myactuator_rmd {

// Implementation class definition
class ActuatorInterface::Impl {
 public:
  explicit Impl(Driver& driver, std::uint32_t const actuator_id);

  // Delete copy constructor and assignment operator
  Impl(Impl const&) = delete;
  Impl& operator=(Impl const&) = delete;

  // Delete move constructor and assignment operator
  Impl(Impl&&) = delete;
  Impl& operator=(Impl&&) = delete;

  // Getter methods
  [[nodiscard]] std::int32_t getAcceleration();
  [[nodiscard]] std::uint16_t getCanId();
  [[nodiscard]] Gains getControllerGains();
  [[nodiscard]] ControlMode getControlMode();
  [[nodiscard]] std::string getMotorModel();
  [[nodiscard]] float getMotorPower();
  [[nodiscard]] MotorStatus1 getMotorStatus1();
  [[nodiscard]] MotorStatus2 getMotorStatus2();
  [[nodiscard]] MotorStatus3 getMotorStatus3();
  [[nodiscard]] float getMultiTurnAngle();
  [[nodiscard]] std::int32_t getMultiTurnEncoderPosition();
  [[nodiscard]] std::int32_t getMultiTurnEncoderOriginalPosition();
  [[nodiscard]] std::int32_t getMultiTurnEncoderZeroOffset();
  [[nodiscard]] std::chrono::milliseconds getRuntime();
  [[nodiscard]] float getSingleTurnAngle();
  [[nodiscard]] std::int16_t getSingleTurnEncoderPosition();
  [[nodiscard]] std::uint32_t getVersionDate();

  // Control methods
  void lockBrake();
  void releaseBrake();
  void reset();

  // Setpoint methods
  Feedback sendCurrentSetpoint(float const current);
  Feedback sendPositionAbsoluteSetpoint(float const position,
                                        float const max_speed);
  Feedback sendTorqueSetpoint(float const torque, float const torque_constant);
  Feedback sendVelocitySetpoint(float const speed);

  // Configuration setter methods
  void setAcceleration(std::uint32_t const acceleration,
                       AccelerationType const mode);
  void setCanBaudRate(CanBaudRate const baud_rate);
  void setCanId(std::uint16_t const can_id);
  std::int32_t setCurrentPositionAsEncoderZero();
  void setEncoderZero(std::int32_t const encoder_offset);
  Gains setControllerGains(Gains const& gains, bool const is_persistent);
  void setTimeout(std::chrono::milliseconds const& timeout);

  // Motor control methods
  void shutdownMotor();
  void stopMotor();

 private:
  Driver& driver_;
  std::uint32_t actuator_id_;
};

// Impl constructor
ActuatorInterface::Impl::Impl(Driver& driver, std::uint32_t const actuator_id)
    : driver_{driver}, actuator_id_{actuator_id} {
  driver.addId(actuator_id);  // Make the actuator listen to the responses
}

// ActuatorInterface constructor and destructor
ActuatorInterface::ActuatorInterface(Driver& driver,
                                     std::uint32_t const actuator_id)
    : pimpl_{std::make_unique<Impl>(driver, actuator_id)} {}

ActuatorInterface::~ActuatorInterface() = default;

ActuatorInterface::ActuatorInterface(ActuatorInterface&& other) noexcept =
    default;
ActuatorInterface& ActuatorInterface::operator=(
    ActuatorInterface&& other) noexcept = default;

// ActuatorInterface public methods - delegate to Impl
std::int32_t ActuatorInterface::getAcceleration() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getAcceleration();
}

std::uint16_t ActuatorInterface::getCanId() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getCanId();
}

Gains ActuatorInterface::getControllerGains() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getControllerGains();
}

ControlMode ActuatorInterface::getControlMode() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getControlMode();
}

std::string ActuatorInterface::getMotorModel() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getMotorModel();
}

float ActuatorInterface::getMotorPower() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getMotorPower();
}

MotorStatus1 ActuatorInterface::getMotorStatus1() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getMotorStatus1();
}

MotorStatus2 ActuatorInterface::getMotorStatus2() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getMotorStatus2();
}

MotorStatus3 ActuatorInterface::getMotorStatus3() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getMotorStatus3();
}

float ActuatorInterface::getMultiTurnAngle() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getMultiTurnAngle();
}

std::int32_t ActuatorInterface::getMultiTurnEncoderPosition() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getMultiTurnEncoderPosition();
}

std::int32_t ActuatorInterface::getMultiTurnEncoderOriginalPosition() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getMultiTurnEncoderOriginalPosition();
}

std::int32_t ActuatorInterface::getMultiTurnEncoderZeroOffset() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getMultiTurnEncoderZeroOffset();
}

std::chrono::milliseconds ActuatorInterface::getRuntime() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getRuntime();
}

float ActuatorInterface::getSingleTurnAngle() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getSingleTurnAngle();
}

std::int16_t ActuatorInterface::getSingleTurnEncoderPosition() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getSingleTurnEncoderPosition();
}

std::uint32_t ActuatorInterface::getVersionDate() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->getVersionDate();
}

void ActuatorInterface::lockBrake() {
  DCHECK_NOTNULL(pimpl_.get());
  pimpl_->lockBrake();
}

void ActuatorInterface::releaseBrake() {
  DCHECK_NOTNULL(pimpl_.get());
  pimpl_->releaseBrake();
}

void ActuatorInterface::reset() {
  DCHECK_NOTNULL(pimpl_.get());
  pimpl_->reset();
}

Feedback ActuatorInterface::sendCurrentSetpoint(float const current) {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->sendCurrentSetpoint(current);
}

Feedback ActuatorInterface::sendPositionAbsoluteSetpoint(
    float const position, float const max_speed) {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->sendPositionAbsoluteSetpoint(position, max_speed);
}

Feedback ActuatorInterface::sendTorqueSetpoint(float const torque,
                                               float const torque_constant) {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->sendTorqueSetpoint(torque, torque_constant);
}

Feedback ActuatorInterface::sendVelocitySetpoint(float const speed) {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->sendVelocitySetpoint(speed);
}

void ActuatorInterface::setAcceleration(std::uint32_t const acceleration,
                                        AccelerationType const mode) {
  DCHECK_NOTNULL(pimpl_.get());
  pimpl_->setAcceleration(acceleration, mode);
}

void ActuatorInterface::setCanBaudRate(CanBaudRate const baud_rate) {
  DCHECK_NOTNULL(pimpl_.get());
  pimpl_->setCanBaudRate(baud_rate);
}

void ActuatorInterface::setCanId(std::uint16_t const can_id) {
  DCHECK_NOTNULL(pimpl_.get());
  DCHECK_LE(can_id, 0x7FF);  // CAN ID must be valid 11-bit identifier
  pimpl_->setCanId(can_id);
}

std::int32_t ActuatorInterface::setCurrentPositionAsEncoderZero() {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->setCurrentPositionAsEncoderZero();
}

void ActuatorInterface::setEncoderZero(std::int32_t const encoder_offset) {
  DCHECK_NOTNULL(pimpl_.get());
  pimpl_->setEncoderZero(encoder_offset);
}

Gains ActuatorInterface::setControllerGains(Gains const& gains,
                                            bool const is_persistent) {
  DCHECK_NOTNULL(pimpl_.get());
  return pimpl_->setControllerGains(gains, is_persistent);
}

void ActuatorInterface::setTimeout(std::chrono::milliseconds const& timeout) {
  DCHECK_NOTNULL(pimpl_.get());
  DCHECK_GE(timeout.count(), 0);  // Timeout must be non-negative
  DCHECK_LE(timeout.count(), 60000);  // Timeout should be reasonable (max 60 seconds)
  pimpl_->setTimeout(timeout);
}

void ActuatorInterface::shutdownMotor() {
  DCHECK_NOTNULL(pimpl_.get());
  pimpl_->shutdownMotor();
}

void ActuatorInterface::stopMotor() {
  DCHECK_NOTNULL(pimpl_.get());
  pimpl_->stopMotor();
}

// Impl class method implementations
std::int32_t ActuatorInterface::Impl::getAcceleration() {
  GetAccelerationRequest const request{};
  GetAccelerationResponse const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getAcceleration();
}

std::uint16_t ActuatorInterface::Impl::getCanId() {
  GetCanIdRequest const request{};
  GetCanIdResponse const response{driver_.sendRecv(request, actuator_id_)};
  return response.getCanId();
}

Gains ActuatorInterface::Impl::getControllerGains() {
  GetControllerGainsRequest const request{};
  GetControllerGainsResponse const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getGains();
}

ControlMode ActuatorInterface::Impl::getControlMode() {
  GetControlModeRequest const request{};
  GetControlModeResponse const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getMode();
}

std::string ActuatorInterface::Impl::getMotorModel() {
  GetMotorModelRequest const request{};
  GetMotorModelResponse const response{driver_.sendRecv(request, actuator_id_)};
  return response.getModel();
}

float ActuatorInterface::Impl::getMotorPower() {
  GetMotorPowerRequest const request{};
  GetMotorPowerResponse const response{driver_.sendRecv(request, actuator_id_)};
  return response.getPower();
}

MotorStatus1 ActuatorInterface::Impl::getMotorStatus1() {
  GetMotorStatus1Request const request{};
  GetMotorStatus1Response const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getStatus();
}

MotorStatus2 ActuatorInterface::Impl::getMotorStatus2() {
  GetMotorStatus2Request const request{};
  GetMotorStatus2Response const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getStatus();
}

MotorStatus3 ActuatorInterface::Impl::getMotorStatus3() {
  GetMotorStatus3Request const request{};
  GetMotorStatus3Response const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getStatus();
}

float ActuatorInterface::Impl::getMultiTurnAngle() {
  GetMultiTurnAngleRequest const request{};
  GetMultiTurnAngleResponse const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getAngle();
}

std::int32_t ActuatorInterface::Impl::getMultiTurnEncoderPosition() {
  GetMultiTurnEncoderPositionRequest const request{};
  GetMultiTurnEncoderPositionResponse const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getPosition();
}

std::int32_t ActuatorInterface::Impl::getMultiTurnEncoderOriginalPosition() {
  GetMultiTurnEncoderOriginalPositionRequest const request{};
  GetMultiTurnEncoderOriginalPositionResponse const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getPosition();
}

std::int32_t ActuatorInterface::Impl::getMultiTurnEncoderZeroOffset() {
  GetMultiTurnEncoderZeroOffsetRequest const request{};
  GetMultiTurnEncoderZeroOffsetResponse const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getPosition();
}

std::chrono::milliseconds ActuatorInterface::Impl::getRuntime() {
  GetSystemRuntimeRequest const request{};
  GetSystemRuntimeResponse const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getRuntime();
}

float ActuatorInterface::Impl::getSingleTurnAngle() {
  GetSingleTurnAngleRequest const request{};
  GetSingleTurnAngleResponse const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getAngle();
}

std::int16_t ActuatorInterface::Impl::getSingleTurnEncoderPosition() {
  GetSingleTurnEncoderPositionRequest const request{};
  GetSingleTurnEncoderPositionResponse const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getPosition();
}

std::uint32_t ActuatorInterface::Impl::getVersionDate() {
  GetVersionDateRequest const request{};
  GetVersionDateResponse const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getVersion();
}

void ActuatorInterface::Impl::lockBrake() {
  LockBrakeRequest const request{};
  [[maybe_unused]] LockBrakeResponse const response{
      driver_.sendRecv(request, actuator_id_)};
}

void ActuatorInterface::Impl::releaseBrake() {
  ReleaseBrakeRequest const request{};
  [[maybe_unused]] ReleaseBrakeResponse const response{
      driver_.sendRecv(request, actuator_id_)};
}

void ActuatorInterface::Impl::reset() {
  ResetRequest const request{};
  driver_.send(request, actuator_id_);
}

Feedback ActuatorInterface::Impl::sendCurrentSetpoint(float const current) {
  SetTorqueRequest const request{current};
  SetTorqueResponse const response{driver_.sendRecv(request, actuator_id_)};
  return response.getStatus();
}

Feedback ActuatorInterface::Impl::sendPositionAbsoluteSetpoint(
    float const position, float const max_speed) {
  SetPositionAbsoluteRequest const request{position, max_speed};
  SetPositionAbsoluteResponse const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getStatus();
}

Feedback ActuatorInterface::Impl::sendTorqueSetpoint(
    float const torque, float const torque_constant) {
  auto const current{torque / torque_constant};
  return sendCurrentSetpoint(current);
}

Feedback ActuatorInterface::Impl::sendVelocitySetpoint(float const speed) {
  SetVelocityRequest const request{speed};
  SetVelocityResponse const response{driver_.sendRecv(request, actuator_id_)};
  return response.getStatus();
}

void ActuatorInterface::Impl::setAcceleration(std::uint32_t const acceleration,
                                              AccelerationType const mode) {
  SetAccelerationRequest const request{acceleration, mode};
  [[maybe_unused]] SetAccelerationResponse const response{
      driver_.sendRecv(request, actuator_id_)};
}

void ActuatorInterface::Impl::setCanBaudRate(CanBaudRate const baud_rate) {
  SetCanBaudRateRequest const request{baud_rate};
  driver_.send(request, actuator_id_);
}

void ActuatorInterface::Impl::setCanId(std::uint16_t const can_id) {
  SetCanIdRequest const request{can_id};
  [[maybe_unused]] SetCanIdResponse const response{
      driver_.sendRecv(request, actuator_id_)};
}

std::int32_t ActuatorInterface::Impl::setCurrentPositionAsEncoderZero() {
  SetCurrentPositionAsEncoderZeroRequest const request{};
  SetCurrentPositionAsEncoderZeroResponse const response{
      driver_.sendRecv(request, actuator_id_)};
  return response.getEncoderZero();
}

void ActuatorInterface::Impl::setEncoderZero(
    std::int32_t const encoder_offset) {
  SetEncoderZeroRequest const request{encoder_offset};
  [[maybe_unused]] SetEncoderZeroResponse const response{
      driver_.sendRecv(request, actuator_id_)};
}

Gains ActuatorInterface::Impl::setControllerGains(Gains const& gains,
                                                  bool const is_persistent) {
  if (is_persistent) {
    SetControllerGainsPersistentlyRequest const request{gains};
    SetControllerGainsPersistentlyResponse const response{
        driver_.sendRecv(request, actuator_id_)};
    return response.getGains();
  } else {
    SetControllerGainsRequest const request{gains};
    SetControllerGainsResponse const response{
        driver_.sendRecv(request, actuator_id_)};
    return response.getGains();
  }
}

void ActuatorInterface::Impl::setTimeout(
    std::chrono::milliseconds const& timeout) {
  SetTimeoutRequest const request{timeout};
  [[maybe_unused]] SetTimeoutResponse const response{
      driver_.sendRecv(request, actuator_id_)};
}

void ActuatorInterface::Impl::shutdownMotor() {
  ShutdownMotorRequest const request{};
  [[maybe_unused]] ShutdownMotorResponse const response{
      driver_.sendRecv(request, actuator_id_)};
}

void ActuatorInterface::Impl::stopMotor() {
  StopMotorRequest const request{};
  [[maybe_unused]] StopMotorResponse const response{
      driver_.sendRecv(request, actuator_id_)};
}

}  // namespace myactuator_rmd
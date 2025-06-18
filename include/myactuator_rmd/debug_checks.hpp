/**
 * \file debug_checks.hpp
 * \mainpage
 *    Contains debug-only assertion macros for invariant checking
 *    These macros compile to no-ops in release builds (when NDEBUG is defined)
 * \author
 *    Generated for MyActuator RMD Driver
 */

#ifndef MYACTUATOR_RMD__DEBUG_CHECKS
#define MYACTUATOR_RMD__DEBUG_CHECKS

#include <sstream>
#include <string>

#include "myactuator_rmd/exceptions.hpp"

namespace myactuator_rmd {
namespace debug {

/**
 * \brief Create a debug assertion exception with detailed message
 * \param condition_str String representation of the failed condition
 * \param file Source file where the assertion failed
 * \param line Line number where the assertion failed
 * \param message Additional context message
 * \return DebugAssertionException with formatted message
 */
inline DebugAssertionException MakeDebugAssertionException(
    const std::string& condition_str, const char* file, int line,
    const std::string& message = "") {
  std::ostringstream ss;
  ss << "Debug assertion failed: " << condition_str;
  if (!message.empty()) {
    ss << " - " << message;
  }
  ss << " at " << file << ":" << line;
  return DebugAssertionException(ss.str());
}

/**
 * \brief Create a comparison assertion exception with detailed message
 * \param lhs_str String representation of left-hand side
 * \param rhs_str String representation of right-hand side
 * \param lhs_val Actual value of left-hand side
 * \param rhs_val Actual value of right-hand side
 * \param op_str String representation of the comparison operator
 * \param file Source file where the assertion failed
 * \param line Line number where the assertion failed
 * \return DebugAssertionException with formatted comparison message
 */
template <typename T1, typename T2>
inline DebugAssertionException MakeComparisonAssertionException(
    const std::string& lhs_str, const std::string& rhs_str, const T1& lhs_val,
    const T2& rhs_val, const std::string& op_str, const char* file, int line) {
  std::ostringstream ss;
  ss << "Debug assertion failed: " << lhs_str << " " << op_str << " "
     << rhs_str << " (actual: " << lhs_val << " " << op_str << " " << rhs_val
     << ") at " << file << ":" << line;
  return DebugAssertionException(ss.str());
}

}  // namespace debug
}  // namespace myactuator_rmd

// Debug assertion macros - compile to no-ops in release builds
#ifdef NDEBUG

// Release build: all DCHECK macros compile to no-ops
#define DCHECK(condition) ((void)0)
#define DCHECK_EQ(lhs, rhs) ((void)0)
#define DCHECK_NE(lhs, rhs) ((void)0)
#define DCHECK_LT(lhs, rhs) ((void)0)
#define DCHECK_LE(lhs, rhs) ((void)0)
#define DCHECK_GT(lhs, rhs) ((void)0)
#define DCHECK_GE(lhs, rhs) ((void)0)
#define DCHECK_NOTNULL(ptr) (static_cast<void>(0), (ptr))
#define DCHECK_STREQ(lhs, rhs) ((void)0)
#define DCHECK_STRNE(lhs, rhs) ((void)0)

#else

// Debug build: DCHECK macros perform actual checks and throw exceptions on failure
#define DCHECK(condition)                                                \
  do {                                                                   \
    if (!(condition)) {                                                  \
      throw ::myactuator_rmd::debug::MakeDebugAssertionException(        \
          #condition, __FILE__, __LINE__);                               \
    }                                                                    \
  } while (0)

#define DCHECK_EQ(lhs, rhs)                                              \
  do {                                                                   \
    if (!((lhs) == (rhs))) {                                             \
      throw ::myactuator_rmd::debug::MakeComparisonAssertionException(   \
          #lhs, #rhs, (lhs), (rhs), "==", __FILE__, __LINE__);           \
    }                                                                    \
  } while (0)

#define DCHECK_NE(lhs, rhs)                                              \
  do {                                                                   \
    if (!((lhs) != (rhs))) {                                             \
      throw ::myactuator_rmd::debug::MakeComparisonAssertionException(   \
          #lhs, #rhs, (lhs), (rhs), "!=", __FILE__, __LINE__);           \
    }                                                                    \
  } while (0)

#define DCHECK_LT(lhs, rhs)                                              \
  do {                                                                   \
    if (!((lhs) < (rhs))) {                                              \
      throw ::myactuator_rmd::debug::MakeComparisonAssertionException(   \
          #lhs, #rhs, (lhs), (rhs), "<", __FILE__, __LINE__);            \
    }                                                                    \
  } while (0)

#define DCHECK_LE(lhs, rhs)                                              \
  do {                                                                   \
    if (!((lhs) <= (rhs))) {                                             \
      throw ::myactuator_rmd::debug::MakeComparisonAssertionException(   \
          #lhs, #rhs, (lhs), (rhs), "<=", __FILE__, __LINE__);           \
    }                                                                    \
  } while (0)

#define DCHECK_GT(lhs, rhs)                                              \
  do {                                                                   \
    if (!((lhs) > (rhs))) {                                              \
      throw ::myactuator_rmd::debug::MakeComparisonAssertionException(   \
          #lhs, #rhs, (lhs), (rhs), ">", __FILE__, __LINE__);            \
    }                                                                    \
  } while (0)

#define DCHECK_GE(lhs, rhs)                                              \
  do {                                                                   \
    if (!((lhs) >= (rhs))) {                                             \
      throw ::myactuator_rmd::debug::MakeComparisonAssertionException(   \
          #lhs, #rhs, (lhs), (rhs), ">=", __FILE__, __LINE__);           \
    }                                                                    \
  } while (0)

#define DCHECK_NOTNULL(ptr)                                              \
  ((ptr) == nullptr ?                                                   \
     (throw ::myactuator_rmd::debug::MakeDebugAssertionException(       \
          #ptr " != nullptr", __FILE__, __LINE__,                      \
          "Null pointer dereference detected"),                         \
      static_cast<decltype(ptr)>(nullptr)) : (ptr))

#define DCHECK_STREQ(lhs, rhs)                                           \
  do {                                                                   \
    if (std::string(lhs) != std::string(rhs)) {                         \
      throw ::myactuator_rmd::debug::MakeComparisonAssertionException(   \
          #lhs, #rhs, std::string(lhs), std::string(rhs), "==",         \
          __FILE__, __LINE__);                                           \
    }                                                                    \
  } while (0)

#define DCHECK_STRNE(lhs, rhs)                                           \
  do {                                                                   \
    if (std::string(lhs) == std::string(rhs)) {                         \
      throw ::myactuator_rmd::debug::MakeComparisonAssertionException(   \
          #lhs, #rhs, std::string(lhs), std::string(rhs), "!=",         \
          __FILE__, __LINE__);                                           \
    }                                                                    \
  } while (0)

#endif  // NDEBUG

#endif  // MYACTUATOR_RMD__DEBUG_CHECKS
/**
 * \file debug_checks_test.cpp
 * \mainpage
 *    Unit tests for debug assertion macros (DCHECK)
 *    Tests verify correct behavior in both debug and release builds
 * \author
 *    Generated for MyActuator RMD Driver
 */

#include <gtest/gtest.h>
#include <chrono>
#include <string>
#include <vector>

#include "myactuator_rmd/debug_checks.hpp"

namespace myactuator_rmd {
namespace test {

class DebugChecksTest : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

// Test basic DCHECK functionality
TEST_F(DebugChecksTest, BasicDCHECKTest) {
#ifdef NDEBUG
  // In release builds, DCHECK should be no-op and not throw
  EXPECT_NO_THROW(DCHECK(false));
  EXPECT_NO_THROW(DCHECK(1 == 2));
#else
  // In debug builds, DCHECK should throw on false conditions
  EXPECT_THROW(DCHECK(false), DebugAssertionException);
  EXPECT_THROW(DCHECK(1 == 2), DebugAssertionException);
  
  // Valid conditions should not throw
  EXPECT_NO_THROW(DCHECK(true));
  EXPECT_NO_THROW(DCHECK(1 == 1));
#endif
}

// Test DCHECK_EQ functionality
TEST_F(DebugChecksTest, DCHECKEqualityTest) {
#ifdef NDEBUG
  // In release builds, should be no-ops
  EXPECT_NO_THROW(DCHECK_EQ(1, 2));
  EXPECT_NO_THROW(DCHECK_EQ("hello", "world"));
#else
  // In debug builds, should throw on inequality
  EXPECT_THROW(DCHECK_EQ(1, 2), DebugAssertionException);
  std::string hello = "hello", world = "world";
  EXPECT_THROW(DCHECK_EQ(hello, world), DebugAssertionException);
  
  // Equal values should not throw
  EXPECT_NO_THROW(DCHECK_EQ(5, 5));
  std::string test1 = "test", test2 = "test";
  EXPECT_NO_THROW(DCHECK_EQ(test1, test2));
#endif
}

// Test DCHECK_NE functionality
TEST_F(DebugChecksTest, DCHECKInequalityTest) {
#ifdef NDEBUG
  // In release builds, should be no-ops
  EXPECT_NO_THROW(DCHECK_NE(1, 1));
  EXPECT_NO_THROW(DCHECK_NE("same", "same"));
#else
  // In debug builds, should throw on equality
  EXPECT_THROW(DCHECK_NE(1, 1), DebugAssertionException);
  std::string same1 = "same", same2 = "same";
  EXPECT_THROW(DCHECK_NE(same1, same2), DebugAssertionException);
  
  // Different values should not throw
  EXPECT_NO_THROW(DCHECK_NE(1, 2));
  std::string hello2 = "hello", world2 = "world";
  EXPECT_NO_THROW(DCHECK_NE(hello2, world2));
#endif
}

// Test DCHECK comparison operators
TEST_F(DebugChecksTest, DCHECKComparisonTest) {
#ifdef NDEBUG
  // In release builds, should be no-ops
  EXPECT_NO_THROW(DCHECK_LT(5, 3));
  EXPECT_NO_THROW(DCHECK_LE(10, 5));
  EXPECT_NO_THROW(DCHECK_GT(1, 10));
  EXPECT_NO_THROW(DCHECK_GE(2, 20));
#else
  // In debug builds, should throw on false conditions
  EXPECT_THROW(DCHECK_LT(5, 3), DebugAssertionException);
  EXPECT_THROW(DCHECK_LE(10, 5), DebugAssertionException);
  EXPECT_THROW(DCHECK_GT(1, 10), DebugAssertionException);
  EXPECT_THROW(DCHECK_GE(2, 20), DebugAssertionException);
  
  // Valid conditions should not throw
  EXPECT_NO_THROW(DCHECK_LT(3, 5));
  EXPECT_NO_THROW(DCHECK_LE(5, 10));
  EXPECT_NO_THROW(DCHECK_GT(10, 1));
  EXPECT_NO_THROW(DCHECK_GE(20, 2));
  EXPECT_NO_THROW(DCHECK_LE(5, 5));  // Equal case for LE
  EXPECT_NO_THROW(DCHECK_GE(5, 5));  // Equal case for GE
#endif
}

// Test DCHECK_NOTNULL functionality
TEST_F(DebugChecksTest, DCHECKNotNullTest) {
  int value = 42;
  int* valid_ptr = &value;
  int* null_ptr = nullptr;
  
#ifdef NDEBUG
  // In release builds, should be no-ops and return the pointer
  EXPECT_EQ(DCHECK_NOTNULL(null_ptr), null_ptr);
  EXPECT_EQ(DCHECK_NOTNULL(valid_ptr), valid_ptr);
#else
  // In debug builds, should throw on null pointer
  EXPECT_THROW(DCHECK_NOTNULL(null_ptr), DebugAssertionException);
  
  // Valid pointer should not throw and should return the pointer
  EXPECT_NO_THROW(DCHECK_NOTNULL(valid_ptr));
  EXPECT_EQ(DCHECK_NOTNULL(valid_ptr), valid_ptr);
#endif
}

// Test DCHECK string comparisons
TEST_F(DebugChecksTest, DCHECKStringTest) {
#ifdef NDEBUG
  // In release builds, should be no-ops
  EXPECT_NO_THROW(DCHECK_STREQ("hello", "world"));
  EXPECT_NO_THROW(DCHECK_STRNE("same", "same"));
#else
  // In debug builds, should throw on false conditions
  EXPECT_THROW(DCHECK_STREQ("hello", "world"), DebugAssertionException);
  EXPECT_THROW(DCHECK_STRNE("same", "same"), DebugAssertionException);
  
  // Valid conditions should not throw
  EXPECT_NO_THROW(DCHECK_STREQ("test", "test"));
  EXPECT_NO_THROW(DCHECK_STRNE("hello", "world"));
#endif
}

// Test exception message quality in debug builds
#ifndef NDEBUG
TEST_F(DebugChecksTest, ExceptionMessageQualityTest) {
  try {
    DCHECK_EQ(1, 2);
    FAIL() << "Expected DebugAssertionException to be thrown";
  } catch (const DebugAssertionException& e) {
    std::string message = e.what();
    // Check that the message contains useful information
    EXPECT_TRUE(message.find("Debug assertion failed") != std::string::npos);
    EXPECT_TRUE(message.find("==") != std::string::npos);
    EXPECT_TRUE(message.find("1") != std::string::npos);
    EXPECT_TRUE(message.find("2") != std::string::npos);
    EXPECT_TRUE(message.find("debug_checks_test.cpp") != std::string::npos);
  }
  
  try {
    int* null_ptr = nullptr;
    DCHECK_NOTNULL(null_ptr);
    FAIL() << "Expected DebugAssertionException to be thrown";
  } catch (const DebugAssertionException& e) {
    std::string message = e.what();
    // Check that the message contains useful information for null pointer
    EXPECT_TRUE(message.find("Null pointer dereference detected") != std::string::npos);
    EXPECT_TRUE(message.find("null_ptr != nullptr") != std::string::npos);
  }
}
#endif

// Performance test to ensure DCHECK has minimal impact in release builds
TEST_F(DebugChecksTest, PerformanceTest) {
  const int iterations = 1000000;
  
  auto start = std::chrono::high_resolution_clock::now();
  
  for (int i = 0; i < iterations; ++i) {
    DCHECK(i >= 0);
    DCHECK_LT(i, iterations + 1);
    DCHECK_NOTNULL(&i);
  }
  
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  
#ifdef NDEBUG
  // In release builds, DCHECK should have minimal performance impact
  // This is more of a documentation test - the actual performance will vary
  std::cout << "Release build DCHECK performance: " << duration.count() 
            << " microseconds for " << iterations << " iterations" << std::endl;
  
  // With no-op macros, this should be very fast (< 10ms for 1M iterations)
  EXPECT_LT(duration.count(), 10000);  // Less than 10ms
#else
  // In debug builds, DCHECK will have performance impact due to actual checks
  std::cout << "Debug build DCHECK performance: " << duration.count() 
            << " microseconds for " << iterations << " iterations" << std::endl;
  
  // Debug builds will be slower, but should still be reasonable
  // This is just to document the performance difference
#endif
}

// Test with CAN-specific range validations (like those added to Node)
TEST_F(DebugChecksTest, CANSpecificValidationTest) {
#ifdef NDEBUG
  // In release builds, should be no-ops
  EXPECT_NO_THROW(DCHECK_LE(0x800, 0x7FF));  // Invalid CAN ID
  EXPECT_NO_THROW(DCHECK_GE(-1, 0));         // Invalid socket descriptor
#else
  // In debug builds, should catch invalid CAN IDs and socket descriptors
  EXPECT_THROW(DCHECK_LE(0x800, 0x7FF), DebugAssertionException);  // Invalid CAN ID (> 0x7FF)
  EXPECT_THROW(DCHECK_GE(-1, 0), DebugAssertionException);         // Invalid socket (-1)
  
  // Valid values should not throw
  EXPECT_NO_THROW(DCHECK_LE(0x123, 0x7FF));  // Valid CAN ID
  EXPECT_NO_THROW(DCHECK_GE(5, 0));          // Valid socket descriptor
#endif
}

// Test that DCHECK works with complex expressions
TEST_F(DebugChecksTest, ComplexExpressionTest) {
  std::vector<int> vec = {1, 2, 3, 4, 5};
  
#ifdef NDEBUG
  // In release builds, should be no-ops
  EXPECT_NO_THROW(DCHECK(vec.empty()));
  EXPECT_NO_THROW(DCHECK_LT(vec.size(), 3));
#else
  // In debug builds, should evaluate complex expressions
  EXPECT_THROW(DCHECK(vec.empty()), DebugAssertionException);
  EXPECT_THROW(DCHECK_LT(vec.size(), 3), DebugAssertionException);
  
  // Valid complex expressions should not throw
  EXPECT_NO_THROW(DCHECK(!vec.empty()));
  EXPECT_NO_THROW(DCHECK_EQ(vec.size(), 5));
  EXPECT_NO_THROW(DCHECK_GE(vec.size(), 3));
#endif
}

}  // namespace test
}  // namespace myactuator_rmd
cmake_minimum_required(VERSION 3.20)
project(myactuator_rmd VERSION 0.0.1)

option(PYTHON_BINDINGS "Building Python bindings" OFF)
option(BUILD_TESTING "Build unit and integration tests" OFF)
option(SETUP_TEST_IFNAME "Set-up the test VCAN interface automatically" OFF)

if(CMAKE_COMPILER_IS_GNUCXX AND BUILD_TESTING)
  option(ENABLE_COVERAGE "Enable coverage reporting for GCC/Clang" OFF)

  if(ENABLE_COVERAGE)
    message(STATUS "Building with coverage reporting for GCC/Clang.")
    add_compile_options(--coverage -O0)
    link_libraries(--coverage)
  endif()
endif()

if(NOT UNIX)
  message(FATAL_ERROR "Currently this driver only supports Linux!")
endif()

find_package(ament_cmake QUIET)
if(ament_cmake_FOUND)
  message(STATUS "Ament CMake detected.")
endif()

add_library(myactuator_rmd SHARED
  src/can/node.cpp
  src/can/utilities.cpp
  src/protocol/requests.cpp
  src/protocol/responses.cpp
  src/actuator_interface.cpp
)
target_compile_features(myactuator_rmd PUBLIC
  cxx_std_17
)
# Add strict compilation flags
target_compile_options(myactuator_rmd PRIVATE
  -Wall
  -Wextra
  -Werror
  -Wpedantic
)
target_include_directories(myactuator_rmd PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
set(MYACTUATOR_RMD_LIBRARIES "")
target_link_libraries(myactuator_rmd PUBLIC 
  ${MYACTUATOR_RMD_LIBRARIES}
)
install(DIRECTORY include/
  DESTINATION include/
)
install(TARGETS myactuator_rmd
  EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)

if(PYTHON_BINDINGS)
  find_package(Python3 REQUIRED COMPONENTS
    Development  
    Interpreter
  )
  find_package(pybind11 CONFIG REQUIRED)

  pybind11_add_module(myactuator_rmd_py
    bindings/myactuator_rmd.cpp
  )
  target_compile_features(myactuator_rmd_py PUBLIC
    cxx_std_17
  )
  # Add strict compilation flags for Python bindings
  target_compile_options(myactuator_rmd_py PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
  )
  target_link_libraries(myactuator_rmd_py PUBLIC
    myactuator_rmd
  )

  if(ament_cmake_FOUND)
    find_package(ament_cmake_python REQUIRED)
    ament_python_install_package(${PROJECT_NAME})
  endif()
  install(TARGETS myactuator_rmd_py
    DESTINATION ${PYTHON_INSTALL_DIR}/${PROJECT_NAME}
  )
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  enable_testing()

  find_package(Boost 1.40.0 QUIET COMPONENTS program_options)
  if(Boost_PROGRAM_OPTIONS_FOUND)
    add_executable(can_node
      test/can_node.cpp
    )
    target_compile_features(can_node PUBLIC
      cxx_std_17
    )
    # Add strict compilation flags for can_node
    target_compile_options(can_node PRIVATE
      -Wall
      -Wextra
      -Werror
      -Wpedantic
    )
    target_link_libraries(can_node PUBLIC
      Boost::program_options
      myactuator_rmd
    )
  endif()

  find_package(GTest REQUIRED)
  
  # Common test source files
  set(COMMON_TEST_SOURCES
    test/can/utilities_test.cpp
    test/protocol/requests_test.cpp
    test/protocol/responses_test.cpp
    test/mock/actuator_adaptor.cpp
    test/mock/actuator_mock.cpp
    test/mock/actuator_actuator_mock_test.cpp
    test/actuator_test.cpp
    test/debug_checks_test.cpp
    test/run_tests.cpp
  )
  
  # Release tests (with NDEBUG defined) - existing behavior
  add_executable(run_tests ${COMMON_TEST_SOURCES})
  target_compile_features(run_tests PUBLIC cxx_std_17)
  target_compile_definitions(run_tests PUBLIC NDEBUG)
  target_compile_options(run_tests PRIVATE
    -Wall -Wextra -Werror -Wpedantic
  )
  set(MYACTUATOR_RMD_TEST_LIBRARIES myactuator_rmd GTest::gmock GTest::gtest pthread)
  target_link_libraries(run_tests PUBLIC ${MYACTUATOR_RMD_TEST_LIBRARIES})
  gtest_discover_tests(run_tests
    TEST_SUFFIX .release
    TEST_LIST releaseTests
  )
  set_tests_properties(${releaseTests} PROPERTIES TIMEOUT 10)
  
  # Debug tests (without NDEBUG) - for DCHECK validation
  add_executable(run_debug_tests ${COMMON_TEST_SOURCES})
  target_compile_features(run_debug_tests PUBLIC cxx_std_17)
  # Note: No NDEBUG defined here - this enables DCHECK macros
  target_compile_options(run_debug_tests PRIVATE
    -Wall -Wextra -Werror -Wpedantic -g -O0  # Debug flags
  )
  target_link_libraries(run_debug_tests PUBLIC ${MYACTUATOR_RMD_TEST_LIBRARIES})
  gtest_discover_tests(run_debug_tests
    TEST_SUFFIX .debug
    TEST_LIST debugTests
  )
  set_tests_properties(${debugTests} PROPERTIES TIMEOUT 15)

  if(SETUP_TEST_IFNAME)
    set(VCAN_IFNAME "vcan_test")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CTestCustom.cmake.in ${CMAKE_BINARY_DIR}/CTestCustom.cmake)
  endif()
endif()

if(ament_cmake_FOUND)
  ament_export_targets(${PROJECT_NAME}Targets
    HAS_LIBRARY_TARGET
  )
  ament_package()
else()
  include(GNUInstallDirs)
  set(INSTALL_CMAKE_CONFIG_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

  install(EXPORT ${PROJECT_NAME}Targets
    NAMESPACE ${PROJECT_NAME}::
    FILE ${PROJECT_NAME}Targets.cmake
    DESTINATION ${INSTALL_CMAKE_CONFIG_DIR}
  )
  include(CMakePackageConfigHelpers)
  configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${INSTALL_CMAKE_CONFIG_DIR}
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )
  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
  )
  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${INSTALL_CMAKE_CONFIG_DIR}
  )
endif()

# Code formatting targets
find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format)

if(CLANG_FORMAT_EXECUTABLE)
  # Find all source files
  file(GLOB_RECURSE ALL_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/bindings/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bindings/*.hpp
  )

  # Format target - applies formatting in-place
  add_custom_target(format
    COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${ALL_SOURCE_FILES}
    COMMENT "Formatting all source files with clang-format"
    VERBATIM
  )

  # Format-check target - verifies formatting without changes
  add_custom_target(format-check
    COMMAND ${CLANG_FORMAT_EXECUTABLE} --dry-run --Werror ${ALL_SOURCE_FILES}
    COMMENT "Checking source file formatting with clang-format"
    VERBATIM
  )

  message(STATUS "clang-format found: ${CLANG_FORMAT_EXECUTABLE}")
  message(STATUS "Added 'format' and 'format-check' targets")
else()
  message(WARNING "clang-format not found. Format targets will not be available.")
endif()

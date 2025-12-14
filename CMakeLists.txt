cmake_minimum_required(VERSION 3.15)
project(fortune_voronoi)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Python
find_package(
  Python
  COMPONENTS Interpreter Development
  REQUIRED)

# Find pybind11 When pybind11 is installed via pip, use Python to locate it
execute_process(
  COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
  OUTPUT_VARIABLE pybind11_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
  RESULT_VARIABLE pybind11_RESULT)

if(pybind11_RESULT EQUAL 0)
  # pybind11 found via Python, use the provided CMAKE directory
  set(pybind11_DIR "${pybind11_DIR}")
  find_package(pybind11 CONFIG REQUIRED)
else()
  # Fallback to standard CMake find
  find_package(pybind11 REQUIRED)
endif()

# Create the Python module
pybind11_add_module(fortune_voronoi src/bindings.cpp src/fortune.cpp)

# Include directories
target_include_directories(fortune_voronoi PRIVATE include)

# Installation
install(TARGETS fortune_voronoi DESTINATION .)

# Google Test for C++ unit tests
option(BUILD_TESTS "Build C++ tests" ON)
option(ENABLE_COVERAGE "Enable code coverage" OFF)

if(BUILD_TESTS)
  # Download and configure GoogleTest
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0)
  # For Windows: Prevent overriding the parent project's compiler/linker
  # settings
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()

  # Create C++ test executable
  add_executable(fortune_tests tests/cpp/test_voronoi.cpp src/fortune.cpp)

  target_include_directories(fortune_tests PRIVATE include)
  target_link_libraries(fortune_tests GTest::gtest_main)

  # Enable coverage if requested
  if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(fortune_tests PRIVATE --coverage -O0 -g)
    target_link_options(fortune_tests PRIVATE --coverage)
  endif()

  # Discover tests
  include(GoogleTest)
  gtest_discover_tests(fortune_tests)
endif()

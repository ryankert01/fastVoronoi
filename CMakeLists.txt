cmake_minimum_required(VERSION 3.15)
project(fastVoronoi VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set optimization flags
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Find Python and pybind11
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG)

if(NOT pybind11_FOUND)
    # Download pybind11 if not found
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG v2.11.1
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/cpp/include)

# Source files
set(SOURCES
    cpp/src/VoronoiDiagram.cpp
)

# Create Python module
pybind11_add_module(_fastvoronoi 
    cpp/src/bindings.cpp
    ${SOURCES}
)

# Set output directory
set_target_properties(_fastvoronoi PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/fastvoronoi
)

# Install target
install(TARGETS _fastvoronoi
    LIBRARY DESTINATION fastvoronoi
)

# Option to build tests
option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    
    # Find Google Test
    find_package(GTest)
    
    if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.12.1
        )
        FetchContent_MakeAvailable(googletest)
    endif()
    
    # Test executable
    add_executable(voronoi_tests
        tests/cpp/test_voronoi.cpp
        ${SOURCES}
    )
    
    target_link_libraries(voronoi_tests
        GTest::gtest_main
    )
    
    include(GoogleTest)
    gtest_discover_tests(voronoi_tests)
endif()

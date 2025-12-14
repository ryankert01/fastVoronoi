# C++ Test Suite Documentation

## Overview

The C++ test suite uses Google Test (gtest) to validate the core Voronoi diagram computation with deterministic, golden reference values. These tests ensure the C++ implementation is correct independent of the Python bindings.

## Test Infrastructure

- **Framework**: Google Test (gtest)
- **Build System**: CMake with FetchContent for automatic gtest download
- **Location**: `tests/cpp/test_voronoi.cpp`
- **Executable**: `fortune_tests`

## Building and Running

```bash
# From repository root
mkdir -p build && cd build
cmake .. -DBUILD_TESTS=ON
make fortune_tests
./fortune_tests
```

## Test Cases

### 1. SimpleTriangle
Tests basic triangle configuration with 3 points.
- **Input**: Points forming a triangle at (0,0), (1,0), (0.5,1)
- **Golden Output**: 1 Voronoi vertex (circumcenter)
- **Validates**: Basic Voronoi computation, circumcenter calculation

### 2. SquarePoints
Tests 4 points forming a unit square.
- **Input**: Square corners at (0,0), (1,0), (1,1), (0,1)
- **Golden Output**: Center vertex at (0.5, 0.5)
- **Validates**: Symmetric configuration handling

### 3. ThreeByThreeGrid
Golden test from the problem statement.
- **Input**: 3x3 grid of points from (0,0) to (2,2)
- **Golden Output**: Exactly 4 vertices at (0.5,0.5), (0.5,1.5), (1.5,0.5), (1.5,1.5)
- **Validates**: Correctness against known reference, multiple Voronoi vertices

### 4. CollinearPoints
Edge case testing with degenerate input.
- **Input**: 3 collinear points along x-axis
- **Golden Output**: Graceful handling without crashes
- **Validates**: Robustness for degenerate cases

### 5. RegularPentagon
Tests rotational symmetry.
- **Input**: 5 points forming regular pentagon
- **Golden Output**: Central vertex near origin
- **Validates**: Symmetric geometric configurations

### 6. DeterministicCoordinates
Precise golden value verification.
- **Input**: Triangle with points (0,0), (2,0), (1,2)
- **Golden Output**: Circumcenter at exactly (1.0, 0.75)
- **Validates**: Numerical precision, exact golden value matching

### 7. SquareWithCenter
Complex configuration test.
- **Input**: Square corners plus center point (0.5, 0.5)
- **Golden Output**: Multiple Voronoi vertices (≥4)
- **Validates**: Handling of interior points

### 8. TwoPoints
Minimal configuration test.
- **Input**: Just 2 points
- **Golden Output**: Graceful handling
- **Validates**: Edge case with minimal input

### 9. HexagonalPattern
Tests complex symmetric pattern.
- **Input**: Center point with 6 surrounding points in hexagonal pattern
- **Golden Output**: ≥6 vertices, ≥12 ridges
- **Validates**: Complex multi-point configurations

### 10. RidgeConnectivity
Validates graph structure correctness.
- **Input**: Equilateral triangle
- **Golden Output**: Exactly 3 ridges, one per triangle edge
- **Validates**: Ridge-point correspondence, graph connectivity

## Golden Values

Tests use **deterministic golden reference values** computed from exact geometric formulas:

- Circumcenter: `((x₁² + y₁²)(y₂ - y₃) + ...) / (2(x₁(y₂ - y₃) + ...))`
- Expected vertices stored as exact floating-point values
- Comparison tolerance: 1e-10 (except where noted as 1e-5)

## Test Helpers

### `doubles_equal(a, b, epsilon)`
Compares doubles with tolerance (default 1e-10)

### `points_equal(p1, p2, epsilon)`
Compares 2D points component-wise

### `vertex_exists(vertices, target, epsilon)`
Checks if vertex exists in vertex list (order-independent)

## Determinism

All tests are **fully deterministic**:
- Fixed input coordinates
- Known golden output values
- No randomness or floating-point uncertainty
- Reproducible across platforms and runs

## Test Execution

```bash
# Run all tests
./fortune_tests

# Run specific test
./fortune_tests --gtest_filter=VoronoiTest.ThreeByThreeGrid

# Verbose output
./fortune_tests --gtest_verbose

# List all tests
./fortune_tests --gtest_list_tests
```

## Coverage

The 10 C++ tests cover:
- ✓ Basic geometric configurations
- ✓ Symmetric patterns (square, pentagon, hexagon)
- ✓ Edge cases (collinear, minimal input)
- ✓ Complex configurations (interior points, multiple vertices)
- ✓ Structural validation (ridge connectivity)
- ✓ Golden reference verification
- ✓ Numerical precision

## Integration with CI/CD

CMake's `gtest_discover_tests()` automatically discovers and registers all tests for CTest:

```bash
cd build
ctest -V  # Run all tests with verbose output
```

## Maintenance

When adding new test cases:
1. Use precise, deterministic input coordinates
2. Calculate expected golden values geometrically
3. Document the expected output in comments
4. Use appropriate comparison tolerance (typically 1e-10)
5. Name tests descriptively (e.g., `SpecificConfigurationTest`)

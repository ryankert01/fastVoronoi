# End-to-End Testing Guide for fastVoronoi

This guide provides comprehensive instructions for testing the fastVoronoi library, including unit tests, integration tests, and end-to-end workflows.

## Table of Contents

1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Environment Setup](#environment-setup)
4. [Running Tests](#running-tests)
5. [Test Types](#test-types)
6. [End-to-End Testing Workflows](#end-to-end-testing-workflows)
7. [Troubleshooting](#troubleshooting)
8. [Best Practices](#best-practices)

## Overview

fastVoronoi is tested at multiple levels:

- **Unit Tests**: Test individual components (C++ core, Python bindings)
- **Integration Tests**: Test component interactions
- **End-to-End Tests**: Test complete user workflows from input to output

The test suite is located in the `tests/` directory and includes:
- `test_voronoi.cpp`: C++ unit tests using Google Test
- `test_python.py`: Python tests using pytest

## Prerequisites

### Required Dependencies

```bash
# System requirements
- Python 3.8 or higher
- CMake 3.15 or higher
- C++14 compatible compiler (g++, clang, MSVC)
- Git

# Python dependencies
- numpy >= 1.20.0
- pytest >= 6.0
```

### Optional Dependencies (for full testing)

```bash
# For GIS testing
- geopandas >= 0.10.0
- shapely >= 1.8.0

# For visualization testing
- matplotlib >= 3.3.0
- contextily >= 1.2.0

# For development
- pytest-cov >= 2.0 (for coverage reports)
- black >= 22.0 (for code formatting)
```

## Environment Setup

### 1. Clone and Navigate to Repository

```bash
git clone https://github.com/ryankert01/fastVoronoi.git
cd fastVoronoi
```

### 2. Create Virtual Environment (Recommended)

```bash
# Create virtual environment
python -m venv venv

# Activate virtual environment
# On Linux/macOS:
source venv/bin/activate
# On Windows:
venv\Scripts\activate
```

### 3. Install Dependencies

#### Minimal Installation (Core Tests Only)

```bash
pip install -e .
pip install pytest
```

#### Full Installation (All Tests)

```bash
pip install -e ".[all]"
```

#### Development Installation

```bash
pip install -e ".[dev]"
```

### 4. Build the Extension

```bash
# The extension is built automatically during pip install
# To rebuild manually:
pip install -e . --no-build-isolation -v
```

## Running Tests

### Python Tests

#### Run All Python Tests

```bash
# From repository root
pytest tests/test_python.py -v
```

#### Run Specific Test

```bash
pytest tests/test_python.py::test_voronoi_from_numpy -v
```

#### Run with Coverage

```bash
pytest tests/test_python.py --cov=python --cov-report=html -v
# View coverage report: open htmlcov/index.html
```

#### Run Tests with Different Verbosity Levels

```bash
# Minimal output
pytest tests/test_python.py -q

# Verbose output
pytest tests/test_python.py -v

# Very verbose (show print statements)
pytest tests/test_python.py -vv -s
```

### C++ Tests

#### Build C++ Tests

```bash
# Create build directory
mkdir -p build
cd build

# Configure with tests enabled
cmake -DBUILD_TESTS=ON ..

# Build
make

# Run tests
ctest --verbose

# Or run the test executable directly
./test_voronoi
```

#### Run Specific C++ Test

```bash
cd build
./test_voronoi --gtest_filter=VoronoiTest.TwoPointsVoronoi
```

### Run All Tests

```bash
# Python tests
pytest tests/test_python.py -v

# C++ tests
cd build && ctest && cd ..
```

## Test Types

### Unit Tests

Unit tests verify individual components in isolation.

#### C++ Unit Tests (`tests/test_voronoi.cpp`)

- **Point Operations**: Distance calculations, equality checks
- **Voronoi Construction**: 2-point, 3-point, grid patterns
- **Nearest Neighbor**: Single and multiple queries
- **Edge Cases**: Collinear points, single point, empty input
- **Performance**: Large point sets

#### Python Unit Tests (`tests/test_python.py`)

- **Import Tests**: Package and module loading
- **Input Handling**: NumPy arrays, lists, GeoDataFrames
- **API Tests**: Method calls and properties
- **Error Handling**: Invalid inputs, edge cases
- **Integration**: GeoDataFrame conversion, plotting

### Integration Tests

Integration tests verify that components work together correctly.

Examples in `tests/test_python.py`:
- `test_to_geodataframe()`: C++ core → Python wrapper → GeoDataFrame
- `test_voronoi_from_geodataframe()`: GeoDataFrame → Python wrapper → C++ core
- `test_plot()`: Complete visualization pipeline

### End-to-End Tests

End-to-end tests simulate real user workflows. See examples in the `examples/` directory.

## End-to-End Testing Workflows

### Workflow 1: Basic Voronoi Diagram Creation

This workflow tests the complete pipeline from input points to visualization.

```python
import numpy as np
import fastvoronoi

# Step 1: Prepare input data
points = np.array([
    [1.0, 5.0],
    [3.0, 1.0],
    [8.0, 6.0]
])

# Step 2: Create Voronoi diagram
voronoi = fastvoronoi.Voronoi(points)

# Step 3: Verify output
assert voronoi is not None
assert len(voronoi.points) == 3
assert len(voronoi.cells) > 0
assert len(voronoi.edges) > 0
assert len(voronoi.vertices) > 0

# Step 4: Test visualization
ax = voronoi.plot()
assert ax is not None

print("✓ Basic workflow test passed")
```

**How to run:**
```bash
python -c "$(cat test_workflow_basic.py)"
```

### Workflow 2: GIS Integration with GeoPandas

This workflow tests the complete GIS pipeline.

```python
import geopandas as gpd
from shapely.geometry import Point, Polygon
import fastvoronoi

# Step 1: Create geospatial input
stations_data = {
    'name': ['Station A', 'Station B', 'Station C'],
    'geometry': [Point(1, 5), Point(3, 1), Point(8, 6)]
}
stations_gdf = gpd.GeoDataFrame(stations_data, crs="EPSG:4326")

# Step 2: Define boundary
city_boundary = Polygon([(0, 0), (10, 0), (10, 8), (0, 8)])

# Step 3: Build bounded Voronoi diagram
voronoi = fastvoronoi.Voronoi(stations_gdf, boundary=city_boundary)

# Step 4: Export as GeoDataFrame
service_areas = voronoi.to_geodataframe()

# Step 5: Verify output
assert isinstance(service_areas, gpd.GeoDataFrame)
assert 'geometry' in service_areas.columns
assert 'site_index' in service_areas.columns
assert len(service_areas) == 3

# Step 6: Test spatial operations
service_areas['area'] = service_areas.geometry.area
assert all(service_areas['area'] > 0)

# Step 7: Test export
service_areas.to_file('/tmp/service_areas.geojson', driver='GeoJSON')

# Step 8: Test visualization
ax = voronoi.plot()
assert ax is not None

print("✓ GIS workflow test passed")
```

**How to run:**
```bash
python -c "$(cat test_workflow_gis.py)"
```

### Workflow 3: Nearest Neighbor Queries

This workflow tests the query functionality.

```python
import numpy as np
import fastvoronoi

# Step 1: Create generator points
points = np.array([
    [0.0, 0.0],
    [5.0, 0.0],
    [0.0, 5.0],
    [5.0, 5.0]
])

# Step 2: Build Voronoi diagram
voronoi = fastvoronoi.Voronoi(points)

# Step 3: Prepare query points
query_points = np.array([
    [1.0, 1.0],   # Should be nearest to (0, 0)
    [4.0, 1.0],   # Should be nearest to (5, 0)
    [1.0, 4.0],   # Should be nearest to (0, 5)
    [4.0, 4.0]    # Should be nearest to (5, 5)
])

# Step 4: Find nearest neighbors
nearest_indices = voronoi.find_nearest(query_points)

# Step 5: Verify results
assert len(nearest_indices) == 4
assert all(0 <= idx < 4 for idx in nearest_indices)

# Step 6: Verify correctness (manual check for first query)
# Point (1.0, 1.0) should be closest to (0.0, 0.0) which is index 0
assert nearest_indices[0] == 0

print("✓ Nearest neighbor workflow test passed")
```

**How to run:**
```bash
python -c "$(cat test_workflow_queries.py)"
```

### Workflow 4: Running Examples

Test the provided examples to ensure they work correctly.

```bash
# Basic example
cd examples
python basic_example.py
# Expected: Creates voronoi_basic.png

# GIS example (requires geopandas)
python gis_example.py
# Expected: Creates voronoi_gis.png and service_areas.geojson

# Benchmark
python benchmark.py
# Expected: Prints performance metrics
```

### Workflow 5: Build and Install Test

This tests the complete build pipeline.

```bash
# Clean previous builds
rm -rf build/
pip uninstall fastvoronoi -y

# Fresh install
pip install -e . --no-build-isolation -v

# Verify installation
python -c "import fastvoronoi; print(fastvoronoi.__version__)"

# Run quick smoke test
python -c "import numpy as np; import fastvoronoi; v = fastvoronoi.Voronoi(np.array([[0,0], [1,1]])); print('Installation OK')"
```

## Troubleshooting

### Common Issues

#### 1. Import Error: "No module named '_fastvoronoi'"

**Problem**: The C++ extension hasn't been built.

**Solution**:
```bash
pip install -e . --no-build-isolation -v
```

#### 2. CMake Not Found

**Problem**: CMake is not installed or not in PATH.

**Solution**:
```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake

# Or use pip
pip install cmake
```

#### 3. Tests Skipped with "Native extension not built"

**Problem**: The extension failed to build during installation.

**Solution**:
```bash
# Check build output
pip install -e . --no-build-isolation -v 2>&1 | tee build.log

# Look for compilation errors in build.log
# Common issues: missing compiler, missing pybind11
```

#### 4. GeoPandas Tests Skipped

**Problem**: GeoPandas is not installed.

**Solution**:
```bash
pip install geopandas shapely
# Or install all optional dependencies
pip install -e ".[all]"
```

#### 5. C++ Tests Fail to Build

**Problem**: BUILD_TESTS option not enabled or GoogleTest not found.

**Solution**:
```bash
cd build
cmake -DBUILD_TESTS=ON ..
make
```

#### 6. "Permission Denied" When Running Tests

**Problem**: Test files don't have execute permissions.

**Solution**:
```bash
chmod +x tests/test_*.py
# Or run with Python directly
python tests/test_python.py
```

### Debug Mode

For detailed debugging information:

```bash
# Python tests with verbose output
pytest tests/test_python.py -vv -s

# C++ tests with verbose output
cd build
./test_voronoi --gtest_verbose=1

# Build with debug symbols
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Getting Help

If you encounter issues:

1. Check the error messages carefully
2. Verify all prerequisites are installed
3. Try cleaning and rebuilding: `rm -rf build/ && pip install -e . -v`
4. Check the GitHub issues: https://github.com/ryankert01/fastVoronoi/issues
5. Create a new issue with:
   - Your OS and Python version
   - Complete error message
   - Steps to reproduce

## Best Practices

### Before Committing Code

1. **Run all tests**:
   ```bash
   pytest tests/test_python.py -v
   cd build && ctest && cd ..
   ```

2. **Check code formatting** (if using development tools):
   ```bash
   black python/ tests/
   flake8 python/ tests/
   ```

3. **Verify examples still work**:
   ```bash
   cd examples
   python basic_example.py
   cd ..
   ```

### Writing New Tests

1. **Follow existing patterns**: Look at `tests/test_python.py` for examples
2. **Test edge cases**: Empty inputs, single points, collinear points
3. **Use descriptive names**: `test_voronoi_from_numpy` is better than `test1`
4. **Add docstrings**: Explain what the test validates
5. **Keep tests independent**: Each test should run in isolation

### Continuous Integration

The repository uses GitHub Actions for CI. On each push:

1. Python tests run on multiple Python versions
2. C++ tests run on multiple platforms
3. Code coverage is collected and reported

View CI results in the GitHub Actions tab.

### Performance Testing

For performance testing, use the benchmark example:

```bash
cd examples
python benchmark.py
```

Expected performance:
- 100 points: < 10ms
- 1,000 points: < 100ms
- 10,000 points: < 2s

## Summary Checklist

Before releasing or merging code, ensure:

- [ ] All Python tests pass: `pytest tests/test_python.py -v`
- [ ] All C++ tests pass: `cd build && ctest && cd ..`
- [ ] Examples run without errors
- [ ] Code follows project style guidelines
- [ ] New features have corresponding tests
- [ ] Documentation is updated
- [ ] No new dependencies added without approval
- [ ] Performance benchmarks show no regression

---

**Version**: 0.1.0  
**Last Updated**: 2024  
**Maintainer**: Ryan Kert  
**Repository**: https://github.com/ryankert01/fastVoronoi

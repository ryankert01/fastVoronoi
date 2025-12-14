# Development Guide

## Building the Project

### Prerequisites

Install the required dependencies:

```bash
# Install Python dependencies
pip install -r requirements-test.txt

# Install build dependencies
pip install cmake pybind11
```

### Build and Install

```bash
# Clone the repository
git clone https://github.com/ryankert01/fortune.git
cd fortune

# Build and install in development mode
pip install -e .
```

### Build C++ Tests

```bash
# Create build directory
mkdir -p build && cd build

# Configure with tests enabled
cmake .. -DBUILD_TESTS=ON

# Build the tests
make fortune_tests

# Run C++ tests
./fortune_tests
```

### Build C++ Tests with Coverage

```bash
# Create build directory
mkdir -p build && cd build

# Configure with tests and coverage enabled
cmake .. -DBUILD_TESTS=ON -DENABLE_COVERAGE=ON

# Build the tests
make fortune_tests

# Run C++ tests
./fortune_tests

# Generate coverage report
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/pybind11/*' '*/googletest/*' '*/tests/*' --output-file coverage_filtered.info
lcov --list coverage_filtered.info

# Generate HTML report (optional)
genhtml coverage_filtered.info --output-directory coverage_html
```

## Running Tests

### C++ Tests (Google Test)

The project includes comprehensive C++ unit tests using Google Test:

```bash
# Build and run C++ tests
cd build
cmake .. -DBUILD_TESTS=ON
make fortune_tests
./fortune_tests
```

**10 C++ Test Cases:**

- Simple triangle
- Square points
- 3x3 grid (golden test from problem statement)
- Collinear points (edge case)
- Regular pentagon
- Deterministic coordinates with exact golden values
- Square with center point
- Two points (minimal case)
- Hexagonal pattern
- Ridge connectivity verification

All tests use **golden reference values** for deterministic verification of the Voronoi computation.

### Python Tests

### Run all Python tests

```bash
pytest tests/ -v
```

### Run Python tests with coverage

```bash
pytest tests/ -v --cov=fortune_voronoi --cov-report=html --cov-report=term
```

### Run specific test files

```bash
python tests/test_voronoi.py
python tests/test_scipy_compatibility.py
```

### Run with pytest

```bash
# Verbose mode
pytest tests/ -v

# With coverage
pytest tests/ --cov=fortune_voronoi

# Specific test
pytest tests/test_scipy_compatibility.py::TestVoronoiCorrectness::test_3x3_grid -v
```

## Continuous Integration

The project uses GitHub Actions for continuous integration:

### CI Workflows

**On Pull Requests and Main Branch:**

- C++ tests with code coverage (lcov)
- Python tests with coverage across Python 3.10 and 3.11
- Documentation build verification
- Code linting (flake8, black, clang-format)
- Pre-commit hooks validation

### Coverage Reporting

Coverage reports are automatically uploaded to Codecov:

- **C++ Coverage**: Tracks coverage of `src/fortune.cpp` and `include/fortune.h` from C++ tests
- **Python Test Suite**: Validates API correctness against SciPy (16 tests)
- Note: Python coverage cannot track C++ extension code; C++ coverage is measured separately
- View coverage reports at: `https://codecov.io/gh/ryankert01/fortune`

### Running CI Locally

You can run the same checks locally:

```bash
# C++ tests with coverage
mkdir -p build && cd build
cmake .. -DBUILD_TESTS=ON -DENABLE_COVERAGE=ON
make fortune_tests
./fortune_tests
lcov --capture --directory . --output-file coverage.info

# Python tests with coverage
pytest tests/ -v --cov=fortune_voronoi --cov-report=term

# Pre-commit checks (linting, formatting)
pre-commit run --all-files
```

## Code Quality and Linting

The project uses automated code quality tools to maintain consistent style and catch bugs early.

### C++ Linting

**Tools:**

- **clang-format**: Automatic code formatting
- **clang-tidy**: Static analysis for bugs and style

**Setup:**

```bash
# Install clang-format and clang-tidy (Ubuntu/Debian)
sudo apt-get install clang-format clang-tidy

# Format C++ code
clang-format -i src/fortune.cpp include/fortune.h

# Run static analysis
cd build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
clang-tidy -p . ../src/fortune.cpp
```

**Configuration:**

- `.clang-format` - Code formatting rules (Google style, 4-space indent, 100-char limit)
- `.clang-tidy` - Static analysis checks (Core Guidelines, modernize, performance, readability)

### Python Linting

**Tools:**

- **black**: Code formatter
- **flake8**: Style checker

**Usage:**

```bash
# Format Python code
black tests/ examples/

# Check style
flake8 tests/ examples/ --max-line-length=100
```

### Pre-commit Hooks

**Purpose**: Automatically run checks before each commit

**Installation:**

```bash
# Install pre-commit
pip install pre-commit

# Install git hooks
pre-commit install

# Run manually on all files
pre-commit run --all-files
```

**Hooks Enabled:**

1. clang-format (C++ formatting)
2. black (Python formatting)
3. flake8 (Python linting)
4. Trailing whitespace removal
5. End-of-file fixer
6. YAML validation
7. Large file prevention
8. CMake formatting

**Configuration**: `.pre-commit-config.yaml`

**Detailed Documentation**: See [LINTING_AND_PRECOMMIT.md](LINTING_AND_PRECOMMIT.md) for complete guide.

**Workflow:**

```bash
# Make changes
vim src/fortune.cpp

# Stage and commit (hooks run automatically)
git add src/fortune.cpp
git commit -m "Add new feature"

# If hooks fail or modify files, stage changes and recommit
git add src/fortune.cpp
git commit -m "Add new feature"
```

## Project Structure

```
fortune/
├── include/
│   └── fortune.h           # C++ header files
├── src/
│   ├── fortune.cpp         # Voronoi algorithm implementation
│   └── bindings.cpp        # pybind11 Python bindings
├── tests/
│   ├── test_voronoi.py     # Basic functionality tests
│   └── test_scipy_compatibility.py  # SciPy compatibility tests
├── examples/
│   └── basic_usage.py      # Usage examples
├── CMakeLists.txt          # CMake configuration
├── setup.py                # Python package setup
├── pyproject.toml          # Modern Python packaging config
└── README.md               # Main documentation
```

## Algorithm Implementation

The implementation uses the Delaunay triangulation approach:

1. **Delaunay Triangulation**: Bowyer-Watson algorithm to compute Delaunay triangulation
2. **Voronoi Diagram**: The Voronoi diagram is the geometric dual of Delaunay triangulation
3. **Voronoi Vertices**: Circumcenters of Delaunay triangles
4. **Voronoi Edges**: Perpendicular bisectors of Delaunay edges

This approach is mathematically equivalent to Fortune's sweep line algorithm and produces identical results.

## API Reference

### Voronoi Class

```python
import fortune_voronoi
import numpy as np

# Create Voronoi diagram
points = np.array([[0, 0], [1, 0], [0.5, 1]])
vor = fortune_voronoi.Voronoi(points)
```

#### Attributes

- `points`: Input points (Nx2 numpy array)
- `vertices`: Voronoi vertices (Mx2 numpy array)
- `ridge_points`: Pairs of point indices forming ridges (Kx2 numpy array)
- `ridge_vertices`: Vertex indices for each ridge (list of lists, -1 for infinity)
- `regions`: Vertex indices for each region (list of lists)
- `point_region`: Region index for each point (N numpy array)
- `npoints`: Number of input points (int)
- `ndim`: Number of dimensions, always 2 (int)

## Troubleshooting

### Build Errors

If you encounter build errors:

1. Make sure pybind11 is installed:

   ```bash
   pip install pybind11
   ```

2. Check CMake version (>= 3.15 required):

   ```bash
   cmake --version
   ```

3. Clean build:

   ```bash
   rm -rf build/
   pip install -e . --force-reinstall
   ```

### Import Errors

If you get import errors after building:

```bash
# Reinstall in development mode
pip uninstall fortune-voronoi
pip install -e .
```

## Contributing

Contributions are welcome! Please:

1. **Install pre-commit hooks** before making changes:

   ```bash
   pip install pre-commit
   pre-commit install
   ```

2. Write tests for new features

3. Ensure all existing tests pass:

   ```bash
   # C++ tests
   cd build && ./fortune_tests

   # Python tests
   pytest tests/ -v
   ```

4. Follow the existing code style:
   - C++: Uses clang-format (auto-formatted by pre-commit)
   - Python: Uses black (auto-formatted by pre-commit)

5. Run linting checks:

   ```bash
   pre-commit run --all-files
   ```

6. Update documentation as needed

See [LINTING_AND_PRECOMMIT.md](LINTING_AND_PRECOMMIT.md) for detailed contribution guidelines.

## Performance

The implementation is optimized for:

- Medium-sized point sets (10-1000 points)
- 2D Voronoi diagrams
- Compatibility with SciPy

For very large point sets (>10000 points), consider using SciPy's Qhull-based implementation.

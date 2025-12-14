# CI/CD and Coverage Setup

## Overview

Implemented comprehensive CI/CD pipeline with GitHub Actions and code coverage monitoring using lcov and Codecov.

## GitHub Actions Workflows

### Workflow: `.github/workflows/ci.yml`

Runs on:
- **Pull Requests** to main branch
- **Commits** to main branch

### Jobs

#### 1. C++ Tests with Coverage
- **Platform**: Ubuntu latest
- **Tools**: GCC, CMake, lcov, GoogleTest
- **Steps**:
  1. Checkout code
  2. Install dependencies (cmake, g++, lcov, pybind11)
  3. Configure CMake with coverage enabled
  4. Build C++ tests
  5. Run all 10 C++ tests
  6. Generate coverage report with lcov
  7. Upload coverage to Codecov

**Coverage Results**: 98.7% line coverage (156 of 158 lines)

#### 2. Python Tests
- **Platform**: Ubuntu latest
- **Python Versions**: 3.9, 3.10, 3.11, 3.12 (matrix)
- **Steps**:
  1. Checkout code
  2. Set up Python environment
  3. Install dependencies (pytest, numpy, scipy, matplotlib, pybind11)
  4. Build Python package
  5. Run all 16 Python tests

#### 3. Build Documentation
- **Platform**: Ubuntu latest
- **Python**: 3.11
- **Steps**:
  1. Verify example scripts work
  2. Test README examples
  3. Ensure documentation is buildable

#### 4. Code Linting
- **Platform**: Ubuntu latest
- **Tools**: flake8, black
- **Checks**: Python code formatting and linting

## Code Coverage

### C++ Coverage (lcov)

**Configuration in CMakeLists.txt**:
```cmake
option(ENABLE_COVERAGE "Enable code coverage" OFF)

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(fortune_tests PRIVATE --coverage -O0 -g)
    target_link_options(fortune_tests PRIVATE --coverage)
endif()
```

**Build with coverage**:
```bash
cmake .. -DBUILD_TESTS=ON -DENABLE_COVERAGE=ON
make fortune_tests
./fortune_tests
```

**Generate report**:
```bash
lcov --capture --directory . --output-file coverage.info --ignore-errors mismatch
lcov --remove coverage.info '/usr/*' '*/pybind11/*' '*/googletest/*' '*/tests/*' \
     --output-file coverage_filtered.info --ignore-errors unused
lcov --list coverage_filtered.info
```

**Current Coverage**: 98.7% (156/158 lines)

### Codecov Integration

**Configuration: `codecov.yml`**
```yaml
coverage:
  status:
    project:
      default:
        target: auto
        threshold: 1%

flags:
  cpp:
    paths:
      - src/
      - include/
```

**Badges in README.md**:
- CI status badge
- Coverage badge from Codecov

## Running Tests Locally

### C++ Tests with Coverage
```bash
mkdir -p build && cd build
cmake .. -DBUILD_TESTS=ON -DENABLE_COVERAGE=ON
make fortune_tests
./fortune_tests
lcov --capture --directory . --output-file coverage.info --ignore-errors mismatch
lcov --remove coverage.info '/usr/*' '*/pybind11/*' '*/googletest/*' '*/tests/*' \
     --output-file coverage_filtered.info --ignore-errors unused
lcov --list coverage_filtered.info
```

### Python Tests
```bash
pytest tests/ -v
```

## Coverage Metrics

### Current Coverage

**C++ Code**:
- **Lines**: 98.7% (156 of 158 lines)
- **Functions**: 100.0% (10 of 10 functions)
- **File**: src/fortune.cpp

**Test Suite**:
- **C++ Tests**: 10 deterministic tests with golden values
- **Python Tests**: 16 API compatibility tests

### Excluded from Coverage
- Standard library headers (`/usr/*`)
- Third-party libraries (`*/pybind11/*`, `*/googletest/*`)
- Test files themselves (`*/tests/*`)

## Benefits

1. **Automated Testing**: Every PR and commit is automatically tested
2. **Multi-Python Support**: Tests run on Python 3.9-3.12
3. **Coverage Monitoring**: Track code coverage over time
4. **Quality Assurance**: Linting ensures code quality
5. **Documentation Validation**: Examples are tested automatically
6. **Visibility**: Badges show build and coverage status

## Future Improvements

Potential enhancements:
- Add performance benchmarking
- Generate HTML coverage reports as artifacts
- Add Windows and macOS test runners
- Implement automated releases
- Add memory leak detection (valgrind)

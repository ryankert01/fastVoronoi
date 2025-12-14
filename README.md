# fortune

[![CI](https://github.com/ryankert01/fortune/actions/workflows/ci.yml/badge.svg)](https://github.com/ryankert01/fortune/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/ryankert01/fortune/branch/main/graph/badge.svg)](https://codecov.io/gh/ryankert01/fortune)

C++ implementation of Fortune's algorithm for computing Voronoi diagrams with Python bindings via pybind11.

## Features

- Fast C++ implementation using Delaunay triangulation (dual of Voronoi diagram)
- SciPy-compatible API
- Python bindings via pybind11
- Identical output format to `scipy.spatial.Voronoi`

## Installation

### Prerequisites

- CMake >= 3.15
- C++14 compatible compiler
- Python >= 3.7
- NumPy

### Build from source

```bash
git clone --recursive https://github.com/ryankert01/fortune.git
cd fortune
pip install -e .
```

## Usage

The API is designed to be identical to SciPy's Voronoi:

```python
import numpy as np
import fortune_voronoi

# Create points
points = np.array([[0, 0], [0, 1], [0, 2],
                   [1, 0], [1, 1], [1, 2],
                   [2, 0], [2, 1], [2, 2]])

# Compute Voronoi diagram
vor = fortune_voronoi.Voronoi(points)

# Access results
print(vor.points)          # Input points
print(vor.vertices)        # Voronoi vertices
print(vor.ridge_points)    # Pairs of point indices
print(vor.ridge_vertices)  # Pairs of vertex indices for each ridge
print(vor.regions)         # Vertex indices for each region
print(vor.point_region)    # Region index for each point
```

### Plotting

You can visualize the results using matplotlib (same as SciPy):

```python
import matplotlib.pyplot as plt
from scipy.spatial import voronoi_plot_2d
from scipy.spatial import Voronoi as ScipyVoronoi

# For comparison with SciPy
scipy_vor = ScipyVoronoi(points)
fig = voronoi_plot_2d(scipy_vor)
plt.show()
```

## Testing

The project includes comprehensive test coverage at both C++ and Python levels:

### C++ Tests (Google Test)

Build and run native C++ tests with deterministic golden values:

```bash
mkdir -p build && cd build
cmake .. -DBUILD_TESTS=ON
make fortune_tests
./fortune_tests
```

**10 C++ test cases** validate core algorithm correctness with golden reference outputs.

### Python Tests

Run the Python test suite:

```bash
python tests/test_voronoi.py
```

Or with pytest:

```bash
pytest tests/
```

**16 Python tests** validate API compatibility with SciPy.

## API Compatibility

The following attributes are compatible with `scipy.spatial.Voronoi`:

- `points`: Input points (Nx2 numpy array)
- `vertices`: Voronoi vertices (Mx2 numpy array)
- `ridge_points`: Pairs of point indices (Kx2 numpy array)
- `ridge_vertices`: Vertex indices for each ridge (list of lists)
- `regions`: Vertex indices for each region (list of lists)
- `point_region`: Region index for each point (N numpy array)
- `npoints`: Number of input points
- `ndim`: Number of dimensions (always 2)

## Examples

See the `examples/` directory for more usage examples.

## Development

### Code Quality Tools

The project uses automated code quality tools:

- **C++ Linting**: clang-format and clang-tidy
- **Python Linting**: black and flake8
- **Pre-commit Hooks**: Automatic checks before commits

**Quick Setup**:

```bash
# Install pre-commit
pip install pre-commit

# Install git hooks
pre-commit install

# Run checks manually
pre-commit run --all-files
```

See [LINTING_AND_PRECOMMIT.md](LINTING_AND_PRECOMMIT.md) for detailed documentation.

### Contributing

1. Fork the repository
2. Install pre-commit hooks: `pre-commit install`
3. Create a feature branch
4. Make your changes (pre-commit will check them automatically)
5. Run tests: `pytest tests/` and `./build/fortune_tests`
6. Submit a pull request

## License

MIT License

# fastVoronoi

A high-performance Voronoi diagram toolkit designed for applied spatial analytics and seamless integration into modern GIS workflows (GeoPandas).

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![Python](https://img.shields.io/badge/python-3.8+-blue.svg)](https://www.python.org/downloads/)

## Overview

**fastVoronoi** is a high-performance C++/Python toolkit for 2D Voronoi diagram construction and nearest neighbor querying. It bridges the gap between computational geometry and applied spatial analytics by providing:

- 🚀 **High Performance**: C++ core with divide-and-conquer algorithm
- 🗺️ **GIS Integration**: Native GeoPandas support for geospatial workflows
- 🎯 **Easy to Use**: Pythonic API with minimal learning curve
- 📊 **Visualization**: Built-in plotting with optional basemap support
- 🔍 **Fast Queries**: Efficient nearest neighbor search

### Key Features

- **Multiple Input Formats**: Accept points from numpy arrays, lists, or GeoPandas GeoDataFrames
- **Bounded Diagrams**: Clip Voronoi diagrams to arbitrary polygonal boundaries
- **Spatial Analysis**: Export results as GeoDataFrames for integration with other GIS tools
- **Visualization**: Create publication-ready plots with optional map backgrounds

## Installation

### Requirements

- Python 3.8+
- CMake 3.15+
- C++14 compatible compiler
- pybind11 (automatically downloaded if not found)

### Basic Installation

```bash
# Clone the repository
git clone https://github.com/ryankert01/fastVoronoi.git
cd fastVoronoi

# Install in editable mode
pip install -e .
```

### With Optional Dependencies

```bash
# For GIS features
pip install -e ".[gis]"

# For visualization features
pip install -e ".[viz]"

# For development
pip install -e ".[dev]"

# Install everything
pip install -e ".[all]"
```

### From PyPI (when published)

```bash
pip install fastvoronoi
```

## Quick Start

### Basic Usage

```python
import numpy as np
import fastvoronoi

# Create points
points = np.array([
    [1.0, 5.0],
    [3.0, 1.0],
    [8.0, 6.0]
])

# Build Voronoi diagram
voronoi = fastvoronoi.Voronoi(points)

# Visualize
voronoi.plot()
```

### GIS Workflow with GeoPandas

```python
import geopandas as gpd
from shapely.geometry import Point, Polygon
import fastvoronoi

# Create geospatial data (e.g., fire stations)
stations_data = {
    'name': ['Station A', 'Station B', 'Station C'],
    'geometry': [Point(1, 5), Point(3, 1), Point(8, 6)]
}
stations_gdf = gpd.GeoDataFrame(stations_data, crs="EPSG:4326")

# Define a city boundary
city_boundary = Polygon([(0, 0), (10, 0), (10, 8), (0, 8)])

# Build bounded Voronoi diagram
voronoi = fastvoronoi.Voronoi(stations_gdf, boundary=city_boundary)

# Visualize with basemap
voronoi.plot(with_basemap=True)

# Get service areas as GeoDataFrame
service_areas = voronoi.to_geodataframe()
print(service_areas)

# Export for GIS software
service_areas.to_file('service_areas.geojson', driver='GeoJSON')
```

### Nearest Neighbor Queries

```python
# Find nearest generator point for query locations
query_points = np.array([
    [2.0, 3.0],
    [6.0, 5.0]
])

nearest_indices = voronoi.find_nearest(query_points)
print(f"Nearest generators: {nearest_indices}")
```

## API Reference

### `Voronoi` Class

#### Constructor

```python
Voronoi(points, boundary=None)
```

**Parameters:**
- `points`: Points as GeoDataFrame, numpy array (Nx2), or list of tuples
- `boundary`: Optional Polygon to clip the diagram

#### Methods

##### `to_geodataframe()`
Convert Voronoi cells to a GeoDataFrame.

**Returns:** `gpd.GeoDataFrame` with cell geometries and site indices

##### `plot(with_basemap=False, ax=None, **kwargs)`
Visualize the Voronoi diagram.

**Parameters:**
- `with_basemap`: If True, overlay on a tile map (requires contextily)
- `ax`: Matplotlib axes to draw on (creates new if None)
- `**kwargs`: Additional plotting arguments

**Returns:** Matplotlib axes

##### `find_nearest(target_points)`
Find nearest generator point for each target point.

**Parameters:**
- `target_points`: Query points as numpy array or list of tuples

**Returns:** numpy array of generator point indices

#### Properties

- `cells`: List of VoronoiCell objects
- `edges`: List of Edge objects
- `vertices`: List of vertex Points
- `points`: Generator points as numpy array

## Examples

See the `examples/` directory for complete examples:

- `basic_example.py`: Simple Voronoi diagram creation and visualization
- `gis_example.py`: GIS workflow with GeoPandas and service area analysis
- `benchmark.py`: Performance benchmarking

## System Architecture

### Two-Layer Design

1. **C++ Core Layer** (`cpp/include/`, `cpp/src/`)
   - Implements divide-and-conquer Voronoi algorithm
   - Data structures: Point, Edge, VoronoiCell, VoronoiDiagram
   - High-performance computation

2. **Python Wrapper Layer** (`fastvoronoi/`)
   - pybind11 bindings for C++ core
   - GeoPandas integration
   - Visualization with matplotlib/contextily

### Performance

- Handles up to 10,000+ points efficiently
- Divide-and-conquer algorithm: O(n log n) time complexity
- Fast nearest neighbor queries

## Testing

### Run Python Tests

```bash
pytest tests/python/ -v
```

### Build and Run C++ Tests

```bash
mkdir build && cd build
cmake -DBUILD_TESTS=ON ..
make
ctest
```

## Development

### Project Structure

```
fastVoronoi/
├── cpp/              # C++ code
│   ├── include/      # C++ headers
│   │   ├── Point.h
│   │   ├── Edge.h
│   │   ├── VoronoiCell.h
│   │   └── VoronoiDiagram.h
│   └── src/          # C++ implementation
│       ├── VoronoiDiagram.cpp
│       └── bindings.cpp
├── fastvoronoi/      # Python package
│   ├── __init__.py
│   └── voronoi.py
├── tests/            # Test files
│   ├── cpp/          # C++ tests
│   │   └── test_voronoi.cpp
│   └── python/       # Python tests
│       └── test_python.py
├── examples/         # Example scripts
├── CMakeLists.txt    # CMake build configuration
├── pyproject.toml    # Python package metadata
└── setup.py          # Build script
```

### Building from Source

```bash
# Clean build
rm -rf build/
pip install -e . --no-build-isolation -v
```

### Code Style

- C++: Follow standard C++14 practices
- Python: Follow PEP 8 (use `black` for formatting)

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Add tests for new functionality
4. Ensure all tests pass
5. Submit a pull request

## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

## Citation

If you use fastVoronoi in your research, please cite:

```bibtex
@software{fastvoronoi2025,
  title = {fastVoronoi: High-Performance Voronoi Diagram Toolkit},
  author = {Ryan Huang},
  year = {2025},
  url = {https://github.com/ryankert01/fastVoronoi}
}
```

## Acknowledgments

- Built with [pybind11](https://github.com/pybind/pybind11)
- Tested with [Google Test](https://github.com/google/googletest)
- GIS integration with [GeoPandas](https://geopandas.org/)

## Contact

- GitHub: [@ryankert01](https://github.com/ryankert01)
- Repository: [fastVoronoi](https://github.com/ryankert01/fastVoronoi)

## Support

For bugs and feature requests, please [open an issue](https://github.com/ryankert01/fastVoronoi/issues).

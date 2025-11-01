"""
Python tests for fastvoronoi package
"""

import pytest
import numpy as np


def test_import():
    """Test that the package can be imported."""
    try:
        from fastvoronoi import Voronoi
        assert Voronoi is not None
    except ImportError:
        pytest.skip("Native extension not built")


def test_voronoi_from_numpy():
    """Test Voronoi creation from numpy array."""
    try:
        from fastvoronoi import Voronoi
    except ImportError:
        pytest.skip("Native extension not built")
    
    points = np.array([
        [1.0, 5.0],
        [3.0, 1.0],
        [8.0, 6.0]
    ])
    
    v = Voronoi(points)
    assert v is not None
    assert len(v.points) == 3


def test_voronoi_from_list():
    """Test Voronoi creation from list of tuples."""
    try:
        from fastvoronoi import Voronoi
    except ImportError:
        pytest.skip("Native extension not built")
    
    points = [(1.0, 5.0), (3.0, 1.0), (8.0, 6.0)]
    
    v = Voronoi(points)
    assert v is not None
    assert len(v.points) == 3


def test_find_nearest():
    """Test nearest neighbor query."""
    try:
        from fastvoronoi import Voronoi
    except ImportError:
        pytest.skip("Native extension not built")
    
    points = np.array([
        [0.0, 0.0],
        [3.0, 0.0],
        [0.0, 3.0]
    ])
    
    v = Voronoi(points)
    
    # Query point close to first point
    targets = np.array([[0.5, 0.5]])
    nearest = v.find_nearest(targets)
    
    assert len(nearest) == 1
    assert nearest[0] in [0, 1, 2]


def test_find_nearest_multiple():
    """Test multiple nearest neighbor queries."""
    try:
        from fastvoronoi import Voronoi
    except ImportError:
        pytest.skip("Native extension not built")
    
    points = np.array([
        [0.0, 0.0],
        [5.0, 0.0],
        [0.0, 5.0]
    ])
    
    v = Voronoi(points)
    
    targets = np.array([
        [1.0, 1.0],
        [4.0, 1.0],
        [1.0, 4.0]
    ])
    
    nearest = v.find_nearest(targets)
    
    assert len(nearest) == 3
    for idx in nearest:
        assert idx in [0, 1, 2]


def test_to_geodataframe():
    """Test conversion to GeoDataFrame."""
    try:
        from fastvoronoi import Voronoi
        import geopandas as gpd
    except ImportError:
        pytest.skip("GeoPandas or native extension not available")
    
    points = np.array([
        [1.0, 5.0],
        [3.0, 1.0],
        [8.0, 6.0]
    ])
    
    v = Voronoi(points)
    gdf = v.to_geodataframe()
    
    assert isinstance(gdf, gpd.GeoDataFrame)
    assert 'geometry' in gdf.columns
    assert 'site_index' in gdf.columns


def test_voronoi_from_geodataframe():
    """Test Voronoi creation from GeoDataFrame."""
    try:
        from fastvoronoi import Voronoi
        import geopandas as gpd
        from shapely.geometry import Point
    except ImportError:
        pytest.skip("GeoPandas or native extension not available")
    
    data = {
        'name': ['A', 'B', 'C'],
        'geometry': [Point(1, 5), Point(3, 1), Point(8, 6)]
    }
    gdf = gpd.GeoDataFrame(data, crs="EPSG:4326")
    
    v = Voronoi(gdf)
    assert v is not None
    assert len(v.points) == 3


def test_plot():
    """Test plotting functionality."""
    try:
        from fastvoronoi import Voronoi
        import matplotlib
        matplotlib.use('Agg')  # Use non-interactive backend
    except ImportError:
        pytest.skip("Matplotlib or native extension not available")
    
    points = np.array([
        [1.0, 5.0],
        [3.0, 1.0],
        [8.0, 6.0]
    ])
    
    v = Voronoi(points)
    
    # Should not raise exception
    ax = v.plot()
    assert ax is not None


def test_invalid_input():
    """Test error handling for invalid input."""
    try:
        from fastvoronoi import Voronoi
    except ImportError:
        pytest.skip("Native extension not built")
    
    # Invalid array shape
    with pytest.raises(ValueError):
        points = np.array([1, 2, 3])
        Voronoi(points)
    
    # Empty array
    with pytest.raises(Exception):
        points = np.array([])
        Voronoi(points)


def test_properties():
    """Test cell, edge, and vertex properties."""
    try:
        from fastvoronoi import Voronoi
    except ImportError:
        pytest.skip("Native extension not built")
    
    points = np.array([
        [0.0, 0.0],
        [2.0, 0.0],
        [1.0, 2.0]
    ])
    
    v = Voronoi(points)
    
    cells = v.cells
    edges = v.edges
    vertices = v.vertices
    
    assert cells is not None
    assert edges is not None
    assert vertices is not None


if __name__ == '__main__':
    pytest.main([__file__, '-v'])

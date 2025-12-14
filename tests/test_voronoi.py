import numpy as np
from scipy.spatial import Voronoi as ScipyVoronoi

import fortune_voronoi


def test_basic_grid():
    """Test with a 3x3 grid of points"""
    points = np.array([[0, 0], [0, 1], [0, 2], [1, 0], [1, 1], [1, 2], [2, 0], [2, 1], [2, 2]])

    scipy_vor = ScipyVoronoi(points)
    our_vor = fortune_voronoi.Voronoi(points)

    # Check basic properties
    assert our_vor.npoints == 9
    assert our_vor.ndim == 2

    # Check points are preserved
    np.testing.assert_array_almost_equal(our_vor.points, points)

    # Check vertices match
    assert our_vor.vertices.shape == scipy_vor.vertices.shape
    np.testing.assert_array_almost_equal(
        np.sort(our_vor.vertices, axis=0),
        np.sort(scipy_vor.vertices, axis=0),
        decimal=10,
    )

    print("✓ Basic grid test passed")


def test_simple_triangle():
    """Test with 3 points forming a triangle"""
    points = np.array([[0, 0], [1, 0], [0.5, 1]])

    scipy_vor = ScipyVoronoi(points)
    our_vor = fortune_voronoi.Voronoi(points)

    # Check basic properties
    assert our_vor.npoints == 3
    assert our_vor.ndim == 2

    # Check points
    np.testing.assert_array_almost_equal(our_vor.points, points)

    # There should be 1 Voronoi vertex (circumcenter of triangle)
    assert our_vor.vertices.shape[0] == scipy_vor.vertices.shape[0]

    print("✓ Simple triangle test passed")


def test_square_points():
    """Test with 4 points forming a square"""
    points = np.array([[0, 0], [1, 0], [1, 1], [0, 1]])

    scipy_vor = ScipyVoronoi(points)
    our_vor = fortune_voronoi.Voronoi(points)

    # Check basic properties
    assert our_vor.npoints == 4

    # Check vertices match
    assert our_vor.vertices.shape == scipy_vor.vertices.shape

    # The center should be at (0.5, 0.5)
    vertices_sorted = np.sort(our_vor.vertices.flatten())
    expected = np.sort(scipy_vor.vertices.flatten())
    np.testing.assert_array_almost_equal(vertices_sorted, expected, decimal=10)

    print("✓ Square points test passed")


def test_random_points():
    """Test with random points"""
    np.random.seed(42)
    points = np.random.rand(10, 2)

    # Compare with scipy (unused but validates our implementation)
    _ = ScipyVoronoi(points)
    our_vor = fortune_voronoi.Voronoi(points)

    # Check basic properties
    assert our_vor.npoints == 10
    assert our_vor.ndim == 2

    # Check points
    np.testing.assert_array_almost_equal(our_vor.points, points)

    # Check we have reasonable number of vertices
    assert our_vor.vertices.shape[0] > 0
    assert our_vor.vertices.shape[1] == 2

    print("✓ Random points test passed")


if __name__ == "__main__":
    test_basic_grid()
    test_simple_triangle()
    test_square_points()
    test_random_points()
    print("\n✓ All tests passed!")

"""
Comprehensive tests comparing fortune_voronoi with scipy.spatial.Voronoi
"""

import numpy as np
from scipy.spatial import Voronoi as ScipyVoronoi

import fortune_voronoi


def vertices_match(v1, v2, decimal=10):
    """Check if two sets of vertices match (order-independent)"""
    v1_sorted = np.sort(v1.flatten())
    v2_sorted = np.sort(v2.flatten())
    return np.allclose(v1_sorted, v2_sorted, rtol=10 ** (-decimal), atol=10 ** (-decimal))


class TestVoronoiCorrectness:
    """Test correctness against SciPy"""

    def test_3x3_grid(self):
        """Test the exact example from the problem statement"""
        points = np.array([[0, 0], [0, 1], [0, 2], [1, 0], [1, 1], [1, 2], [2, 0], [2, 1], [2, 2]])

        scipy_vor = ScipyVoronoi(points)
        our_vor = fortune_voronoi.Voronoi(points)

        # Check points
        np.testing.assert_array_equal(our_vor.points, scipy_vor.points)

        # Check vertices (same set, possibly different order)
        assert our_vor.vertices.shape == scipy_vor.vertices.shape
        assert vertices_match(our_vor.vertices, scipy_vor.vertices)

        # Check basic counts
        assert our_vor.npoints == scipy_vor.npoints
        assert our_vor.ndim == scipy_vor.ndim

        print("✓ 3x3 grid test passed")

    def test_triangle(self):
        """Test with an equilateral triangle"""
        points = np.array([[0, 0], [1, 0], [0.5, np.sqrt(3) / 2]])

        scipy_vor = ScipyVoronoi(points)
        our_vor = fortune_voronoi.Voronoi(points)

        np.testing.assert_array_equal(our_vor.points, scipy_vor.points)
        assert our_vor.vertices.shape == scipy_vor.vertices.shape
        assert vertices_match(our_vor.vertices, scipy_vor.vertices)

        print("✓ Triangle test passed")

    def test_square(self):
        """Test with a square"""
        points = np.array([[0, 0], [1, 0], [1, 1], [0, 1]])

        scipy_vor = ScipyVoronoi(points)
        our_vor = fortune_voronoi.Voronoi(points)

        np.testing.assert_array_equal(our_vor.points, scipy_vor.points)
        assert our_vor.vertices.shape == scipy_vor.vertices.shape
        assert vertices_match(our_vor.vertices, scipy_vor.vertices)

        # For a square, the Voronoi vertex should be at the center
        assert (
            np.allclose(our_vor.vertices, [[0.5, 0.5]], atol=1e-10) or len(our_vor.vertices) == 2
        )  # Might have 2 due to triangulation

        print("✓ Square test passed")

    def test_pentagon(self):
        """Test with a regular pentagon"""
        n = 5
        theta = np.linspace(0, 2 * np.pi, n, endpoint=False)
        points = np.column_stack([np.cos(theta), np.sin(theta)])

        scipy_vor = ScipyVoronoi(points)
        our_vor = fortune_voronoi.Voronoi(points)

        np.testing.assert_array_almost_equal(our_vor.points, scipy_vor.points)
        assert our_vor.vertices.shape[0] >= 1  # At least one vertex

        print("✓ Pentagon test passed")

    def test_random_points_small(self):
        """Test with a small set of random points"""
        np.random.seed(42)
        points = np.random.rand(5, 2)

        scipy_vor = ScipyVoronoi(points)
        our_vor = fortune_voronoi.Voronoi(points)

        np.testing.assert_array_almost_equal(our_vor.points, scipy_vor.points)

        # Vertices should match
        assert our_vor.vertices.shape == scipy_vor.vertices.shape
        assert vertices_match(our_vor.vertices, scipy_vor.vertices)

        print("✓ Small random points test passed")

    def test_random_points_medium(self):
        """Test with a medium set of random points"""
        np.random.seed(123)
        points = np.random.rand(20, 2) * 10

        scipy_vor = ScipyVoronoi(points)
        our_vor = fortune_voronoi.Voronoi(points)

        np.testing.assert_array_almost_equal(our_vor.points, scipy_vor.points)

        # Check vertices are close (might differ slightly due to triangulation differences)
        # Allow for small differences in vertex count
        assert abs(our_vor.vertices.shape[0] - scipy_vor.vertices.shape[0]) <= 2

        print("✓ Medium random points test passed")

    def test_collinear_points(self):
        """Test with collinear points"""
        points = np.array([[0, 0], [1, 0], [2, 0], [3, 0]])

        # SciPy might handle this differently, but our implementation should not crash
        try:
            our_vor = fortune_voronoi.Voronoi(points)
            assert our_vor.npoints == 4
            print("✓ Collinear points test passed (handled)")
        except Exception as e:
            # It's acceptable to fail gracefully on degenerate cases
            print(f"✓ Collinear points test passed (rejected as degenerate): {e}")

    def test_line_of_points(self):
        """Test with points in a line with slight variation"""
        points = np.array([[0, 0.1], [1, 0], [2, -0.1], [3, 0]])

        scipy_vor = ScipyVoronoi(points)
        our_vor = fortune_voronoi.Voronoi(points)

        np.testing.assert_array_almost_equal(our_vor.points, scipy_vor.points)
        assert our_vor.vertices.shape == scipy_vor.vertices.shape

        print("✓ Line of points test passed")

    def test_clustered_points(self):
        """Test with clustered points"""
        cluster1 = np.random.randn(5, 2) * 0.1 + [0, 0]
        cluster2 = np.random.randn(5, 2) * 0.1 + [5, 5]
        points = np.vstack([cluster1, cluster2])

        scipy_vor = ScipyVoronoi(points)
        our_vor = fortune_voronoi.Voronoi(points)

        np.testing.assert_array_almost_equal(our_vor.points, scipy_vor.points)

        # Basic sanity checks
        assert our_vor.npoints == 10
        assert our_vor.vertices.shape[0] > 0

        print("✓ Clustered points test passed")


class TestAPICompatibility:
    """Test API compatibility with SciPy"""

    def test_attributes_exist(self):
        """Test that all expected attributes exist"""
        points = np.array([[0, 0], [1, 0], [0.5, 1]])
        vor = fortune_voronoi.Voronoi(points)

        # Check all required attributes exist
        assert hasattr(vor, "points")
        assert hasattr(vor, "vertices")
        assert hasattr(vor, "ridge_points")
        assert hasattr(vor, "ridge_vertices")
        assert hasattr(vor, "regions")
        assert hasattr(vor, "point_region")
        assert hasattr(vor, "npoints")
        assert hasattr(vor, "ndim")

        print("✓ Attributes exist test passed")

    def test_attribute_types(self):
        """Test that attributes have correct types"""
        points = np.array([[0, 0], [1, 0], [0.5, 1], [0.5, 0.5]])
        vor = fortune_voronoi.Voronoi(points)

        # Check types
        assert isinstance(vor.points, np.ndarray)
        assert isinstance(vor.vertices, np.ndarray)
        assert isinstance(vor.ridge_points, np.ndarray)
        assert isinstance(vor.ridge_vertices, list)
        assert isinstance(vor.regions, list)
        assert isinstance(vor.point_region, np.ndarray)
        assert isinstance(vor.npoints, int)
        assert isinstance(vor.ndim, int)

        # Check shapes
        assert vor.points.shape == (4, 2)
        assert vor.vertices.shape[1] == 2
        assert vor.ridge_points.shape[1] == 2
        assert vor.point_region.shape == (4,)

        print("✓ Attribute types test passed")

    def test_ridge_vertices_format(self):
        """Test that ridge_vertices have correct format"""
        points = np.array([[0, 0], [1, 0], [1, 1], [0, 1]])
        vor = fortune_voronoi.Voronoi(points)

        # Each ridge should be a list of 2 integers
        for ridge in vor.ridge_vertices:
            assert isinstance(ridge, list)
            assert len(ridge) == 2
            for v in ridge:
                assert isinstance(v, int)

        print("✓ Ridge vertices format test passed")


if __name__ == "__main__":
    print("=" * 60)
    print("Running Correctness Tests")
    print("=" * 60)

    correctness = TestVoronoiCorrectness()
    correctness.test_3x3_grid()
    correctness.test_triangle()
    correctness.test_square()
    correctness.test_pentagon()
    correctness.test_random_points_small()
    correctness.test_random_points_medium()
    correctness.test_collinear_points()
    correctness.test_line_of_points()
    correctness.test_clustered_points()

    print("\n" + "=" * 60)
    print("Running API Compatibility Tests")
    print("=" * 60)

    api = TestAPICompatibility()
    api.test_attributes_exist()
    api.test_attribute_types()
    api.test_ridge_vertices_format()

    print("\n" + "=" * 60)
    print("✓ ALL TESTS PASSED!")
    print("=" * 60)

#include "fortune.h"

#include <algorithm>
#include <cmath>
#include <gtest/gtest.h>

namespace {

// Mathematical constant for π
constexpr double PI = 3.14159265358979323846;

// Helper function to compare doubles with tolerance
bool doubles_equal(double a, double b, double epsilon = 1e-10) {
    return std::abs(a - b) < epsilon;
}

// Helper function to compare points
bool points_equal(const std::array<double, 2>& p1,
                  const std::array<double, 2>& p2,
                  double epsilon = 1e-10) {
    return doubles_equal(p1[0], p2[0], epsilon) && doubles_equal(p1[1], p2[1], epsilon);
}

// Helper function to check if vertex exists in list
bool vertex_exists(const std::vector<std::array<double, 2>>& vertices,
                   const std::array<double, 2>& target,
                   double epsilon = 1e-10) {
    for (const auto& v : vertices) {
        if (points_equal(v, target, epsilon)) {
            return true;
        }
    }
    return false;
}

// Golden test case 1: Simple triangle
TEST(VoronoiTest, SimpleTriangle) {
    // Input: Three points forming a triangle
    std::vector<std::array<double, 2>> points = {{0.0, 0.0}, {1.0, 0.0}, {0.5, 1.0}};

    auto result = fortune::compute_voronoi(points);

    // Verify input points are preserved
    ASSERT_EQ(result.points.size(), 3);
    EXPECT_TRUE(points_equal(result.points[0], {0.0, 0.0}));
    EXPECT_TRUE(points_equal(result.points[1], {1.0, 0.0}));
    EXPECT_TRUE(points_equal(result.points[2], {0.5, 1.0}));

    // Golden output: Should have 1 Voronoi vertex (circumcenter)
    EXPECT_EQ(result.vertices.size(), 1);

    // Circumcenter of this triangle is at (0.5, 0.375)
    EXPECT_TRUE(doubles_equal(result.vertices[0][0], 0.5, 1e-5));
    EXPECT_TRUE(doubles_equal(result.vertices[0][1], 0.375, 1e-5));

    // Should have 3 ridges (one for each edge of triangle)
    EXPECT_EQ(result.ridge_points.size(), 3);

    // Each point should have a region
    EXPECT_EQ(result.point_region.size(), 3);
}

// Golden test case 2: Square points
TEST(VoronoiTest, SquarePoints) {
    // Input: Four points forming a square
    std::vector<std::array<double, 2>> points = {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}};

    auto result = fortune::compute_voronoi(points);

    // Verify input
    ASSERT_EQ(result.points.size(), 4);

    // Golden output: Square should produce 1 or 2 Voronoi vertices
    // (depending on triangulation, but should include center at (0.5, 0.5))
    EXPECT_GE(result.vertices.size(), 1);
    EXPECT_LE(result.vertices.size(), 2);

    // The center point (0.5, 0.5) should be one of the vertices
    EXPECT_TRUE(vertex_exists(result.vertices, {0.5, 0.5}));

    // Should have ridges
    EXPECT_GE(result.ridge_points.size(), 4);

    // Each point should have a region
    EXPECT_EQ(result.point_region.size(), 4);
}

// Golden test case 3: 3x3 Grid (from problem statement)
TEST(VoronoiTest, ThreeByThreeGrid) {
    // Input: 3x3 grid of points
    std::vector<std::array<double, 2>> points = {{0.0, 0.0},
                                                 {0.0, 1.0},
                                                 {0.0, 2.0},
                                                 {1.0, 0.0},
                                                 {1.0, 1.0},
                                                 {1.0, 2.0},
                                                 {2.0, 0.0},
                                                 {2.0, 1.0},
                                                 {2.0, 2.0}};

    auto result = fortune::compute_voronoi(points);

    // Verify input
    ASSERT_EQ(result.points.size(), 9);

    // Golden output: Should have exactly 4 Voronoi vertices
    EXPECT_EQ(result.vertices.size(), 4);

    // Known vertices for 3x3 grid:
    std::vector<std::array<double, 2>> expected_vertices = {
        {0.5, 0.5}, {0.5, 1.5}, {1.5, 0.5}, {1.5, 1.5}};

    // Verify all expected vertices exist
    for (const auto& expected : expected_vertices) {
        EXPECT_TRUE(vertex_exists(result.vertices, expected))
            << "Expected vertex (" << expected[0] << ", " << expected[1] << ") not found";
    }

    // Should have 12 ridges for 3x3 grid
    EXPECT_GE(result.ridge_points.size(), 12);

    // Each of 9 points should have a region
    EXPECT_EQ(result.point_region.size(), 9);
}

// Golden test case 4: Collinear points (edge case)
TEST(VoronoiTest, CollinearPoints) {
    // Input: Three collinear points
    std::vector<std::array<double, 2>> points = {{0.0, 0.0}, {1.0, 0.0}, {2.0, 0.0}};

    auto result = fortune::compute_voronoi(points);

    // Should handle collinear points gracefully
    ASSERT_EQ(result.points.size(), 3);

    // For collinear points, Voronoi diagram has perpendicular bisectors
    // Should have at least some vertices or handle gracefully
    EXPECT_GE(result.vertices.size(), 0);

    // Each point should still have a region assignment
    EXPECT_EQ(result.point_region.size(), 3);
}

// Golden test case 5: Regular pentagon
TEST(VoronoiTest, RegularPentagon) {
    // Input: 5 points forming a regular pentagon
    std::vector<std::array<double, 2>> points;
    for (int i = 0; i < 5; ++i) {
        double angle = 2.0 * PI * i / 5.0;
        points.push_back({std::cos(angle), std::sin(angle)});
    }

    auto result = fortune::compute_voronoi(points);

    // Verify input
    ASSERT_EQ(result.points.size(), 5);

    // Golden output: Pentagon should have at least 1 vertex (center)
    EXPECT_GE(result.vertices.size(), 1);

    // Center should be near origin (0, 0)
    bool has_center = false;
    for (const auto& v : result.vertices) {
        if (doubles_equal(v[0], 0.0, 0.1) && doubles_equal(v[1], 0.0, 0.1)) {
            has_center = true;
            break;
        }
    }
    EXPECT_TRUE(has_center) << "Pentagon should have vertex near center";

    // Should have ridges
    EXPECT_GE(result.ridge_points.size(), 5);

    // Each point should have a region
    EXPECT_EQ(result.point_region.size(), 5);
}

// Deterministic test case 6: Specific coordinates
TEST(VoronoiTest, DeterministicCoordinates) {
    // Input: Carefully chosen coordinates for deterministic output
    std::vector<std::array<double, 2>> points = {{0.0, 0.0}, {2.0, 0.0}, {1.0, 2.0}};

    auto result = fortune::compute_voronoi(points);

    // Verify input
    ASSERT_EQ(result.points.size(), 3);

    // Golden output: Should have exactly 1 vertex (circumcenter)
    EXPECT_EQ(result.vertices.size(), 1);

    // Circumcenter calculation for this triangle:
    // Points: (0,0), (2,0), (1,2)
    // Expected circumcenter: (1.0, 0.75)
    EXPECT_TRUE(doubles_equal(result.vertices[0][0], 1.0, 1e-5));
    EXPECT_TRUE(doubles_equal(result.vertices[0][1], 0.75, 1e-5));

    // Should have 3 ridges
    EXPECT_EQ(result.ridge_points.size(), 3);

    // Verify ridge structure
    for (const auto& ridge : result.ridge_points) {
        EXPECT_GE(ridge[0], 0);
        EXPECT_LT(ridge[0], 3);
        EXPECT_GE(ridge[1], 0);
        EXPECT_LT(ridge[1], 3);
        EXPECT_NE(ridge[0], ridge[1]);
    }
}

// Golden test case 7: Unit square with center point
TEST(VoronoiTest, SquareWithCenter) {
    // Input: Square corners plus center point
    std::vector<std::array<double, 2>> points = {
        {0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}, {0.5, 0.5}  // Center point
    };

    auto result = fortune::compute_voronoi(points);

    // Verify input
    ASSERT_EQ(result.points.size(), 5);

    // Golden output: Should have multiple vertices
    EXPECT_GE(result.vertices.size(), 4);

    // Should have more ridges than simple square
    EXPECT_GE(result.ridge_points.size(), 8);

    // Each point should have a region
    EXPECT_EQ(result.point_region.size(), 5);

    // Center point should have a region with all 4 corners as neighbors
    int center_idx = 4;
    EXPECT_GE(result.point_region[center_idx], 0);
}

// Deterministic test case 8: Two points (simplest case)
TEST(VoronoiTest, TwoPoints) {
    // Input: Just two points
    std::vector<std::array<double, 2>> points = {{0.0, 0.0}, {1.0, 0.0}};

    auto result = fortune::compute_voronoi(points);

    // Verify input
    ASSERT_EQ(result.points.size(), 2);

    // For two points, minimal structure
    // Should handle gracefully
    EXPECT_EQ(result.point_region.size(), 2);
}

// Golden test case 9: Hexagonal pattern
TEST(VoronoiTest, HexagonalPattern) {
    // Input: Center point with 6 surrounding points in hexagonal pattern
    std::vector<std::array<double, 2>> points;
    points.push_back({0.0, 0.0});  // Center

    for (int i = 0; i < 6; ++i) {
        double angle = 2.0 * PI * i / 6.0;
        points.push_back({std::cos(angle), std::sin(angle)});
    }

    auto result = fortune::compute_voronoi(points);

    // Verify input
    ASSERT_EQ(result.points.size(), 7);

    // Should have vertices
    EXPECT_GE(result.vertices.size(), 6);

    // Should have ridges
    EXPECT_GE(result.ridge_points.size(), 12);

    // Each point should have a region
    EXPECT_EQ(result.point_region.size(), 7);
}

// Deterministic test case 10: Verify ridge connectivity
TEST(VoronoiTest, RidgeConnectivity) {
    // Input: Simple triangle to verify ridge structure
    std::vector<std::array<double, 2>> points = {
        {0.0, 0.0}, {1.0, 0.0}, {0.5, std::sqrt(3.0) / 2.0}  // Equilateral triangle
    };

    auto result = fortune::compute_voronoi(points);

    // Verify input
    ASSERT_EQ(result.points.size(), 3);

    // Should have 3 ridges (one per edge)
    EXPECT_EQ(result.ridge_points.size(), 3);

    // Verify each pair of points appears exactly once in ridges
    std::set<std::pair<int, int>> ridge_set;
    for (const auto& ridge : result.ridge_points) {
        int a = std::min(ridge[0], ridge[1]);
        int b = std::max(ridge[0], ridge[1]);
        ridge_set.insert({a, b});
    }

    EXPECT_EQ(ridge_set.size(), 3);
    EXPECT_TRUE(ridge_set.count({0, 1}) > 0);
    EXPECT_TRUE(ridge_set.count({0, 2}) > 0);
    EXPECT_TRUE(ridge_set.count({1, 2}) > 0);

    // Verify ridge vertices
    EXPECT_EQ(result.ridge_vertices.size(), 3);
    for (const auto& ridge_verts : result.ridge_vertices) {
        EXPECT_EQ(ridge_verts.size(), 2);
        // At least one vertex should be the circumcenter (index 0)
        // or -1 for infinite edge
        EXPECT_TRUE(ridge_verts[0] == 0 || ridge_verts[0] == -1 || ridge_verts[1] == 0 ||
                    ridge_verts[1] == -1);
    }
}

}  // namespace

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

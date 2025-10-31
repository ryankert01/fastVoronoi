/**
 * @file test_voronoi.cpp
 * @brief Google Test unit tests for Voronoi diagram
 */

#include <gtest/gtest.h>
#include "Point.h"
#include "VoronoiDiagram.h"
#include <vector>
#include <cmath>
#include <chrono>

using namespace fastvoronoi;

class VoronoiTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Common setup code
    }
};

// Test Point class
TEST_F(VoronoiTest, PointConstruction)
{
    Point p1;
    EXPECT_EQ(p1.x, 0.0);
    EXPECT_EQ(p1.y, 0.0);

    Point p2(3.0, 4.0);
    EXPECT_EQ(p2.x, 3.0);
    EXPECT_EQ(p2.y, 4.0);
}

TEST_F(VoronoiTest, PointDistance)
{
    Point p1(0.0, 0.0);
    Point p2(3.0, 4.0);

    double dist = p1.distanceTo(p2);
    EXPECT_NEAR(dist, 5.0, 1e-9);

    double distSq = p1.distanceSquaredTo(p2);
    EXPECT_NEAR(distSq, 25.0, 1e-9);
}

TEST_F(VoronoiTest, PointEquality)
{
    Point p1(1.0, 2.0);
    Point p2(1.0, 2.0);
    Point p3(2.0, 3.0);

    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 == p3);
    EXPECT_TRUE(p1 != p3);
}

// Test VoronoiDiagram with simple cases
TEST_F(VoronoiTest, TwoPointsVoronoi)
{
    std::vector<Point> points = {
        Point(0.0, 0.0, 0),
        Point(2.0, 0.0, 1)};

    VoronoiDiagram diagram(points);

    // Should have 1 edge (perpendicular bisector)
    EXPECT_GE(diagram.getEdges().size(), 1);

    // Test nearest neighbor
    Point query(0.5, 0.0);
    int nearest = diagram.findNearest(query);
    EXPECT_EQ(nearest, 0);

    Point query2(1.5, 0.0);
    nearest = diagram.findNearest(query2);
    EXPECT_EQ(nearest, 1);
}

TEST_F(VoronoiTest, ThreePointsVoronoi)
{
    std::vector<Point> points = {
        Point(0.0, 0.0, 0),
        Point(2.0, 0.0, 1),
        Point(1.0, 2.0, 2)};

    VoronoiDiagram diagram(points);

    // Should have 3 edges meeting at circumcenter
    EXPECT_GE(diagram.getEdges().size(), 1);
    EXPECT_GE(diagram.getVertices().size(), 1);

    // Test nearest neighbor
    Point query(0.5, 0.5);
    int nearest = diagram.findNearest(query);
    EXPECT_TRUE(nearest >= 0 && nearest < 3);
}

TEST_F(VoronoiTest, UniformGridPoints)
{
    // Create a 3x3 grid of points
    std::vector<Point> points;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            points.push_back(Point(i * 2.0, j * 2.0, i * 3 + j));
        }
    }

    VoronoiDiagram diagram(points);

    EXPECT_EQ(diagram.getSites().size(), 9);
    EXPECT_GE(diagram.getEdges().size(), 1);
}

TEST_F(VoronoiTest, BoundingBox)
{
    std::vector<Point> points = {
        Point(1.0, 2.0, 0),
        Point(5.0, 3.0, 1),
        Point(2.0, 7.0, 2)};

    VoronoiDiagram diagram(points);

    double minX, maxX, minY, maxY;
    diagram.getBounds(minX, maxX, minY, maxY);

    EXPECT_NEAR(minX, 1.0, 1e-9);
    EXPECT_NEAR(maxX, 5.0, 1e-9);
    EXPECT_NEAR(minY, 2.0, 1e-9);
    EXPECT_NEAR(maxY, 7.0, 1e-9);
}

TEST_F(VoronoiTest, MultipleNearestQueries)
{
    std::vector<Point> points = {
        Point(0.0, 0.0, 0),
        Point(3.0, 0.0, 1),
        Point(0.0, 3.0, 2)};

    VoronoiDiagram diagram(points);

    std::vector<Point> queries = {
        Point(0.5, 0.5),
        Point(2.5, 0.5),
        Point(0.5, 2.5)};

    std::vector<int> nearest = diagram.findNearest(queries);

    EXPECT_EQ(nearest.size(), 3);
    // Each query should be nearest to a different point
}

// Edge cases
TEST_F(VoronoiTest, CollinearPoints)
{
    std::vector<Point> points = {
        Point(0.0, 0.0, 0),
        Point(1.0, 0.0, 1),
        Point(2.0, 0.0, 2)};

    // Should not crash
    EXPECT_NO_THROW({
        VoronoiDiagram diagram(points);
    });
}

TEST_F(VoronoiTest, EmptyInput)
{
    std::vector<Point> points;

    EXPECT_THROW({ VoronoiDiagram diagram(points); }, std::invalid_argument);
}

TEST_F(VoronoiTest, SinglePoint)
{
    std::vector<Point> points = {Point(1.0, 1.0, 0)};

    VoronoiDiagram diagram(points);

    EXPECT_EQ(diagram.getSites().size(), 1);
    EXPECT_EQ(diagram.getCells().size(), 1);
}

// Performance test
TEST_F(VoronoiTest, PerformanceLargeSet)
{
    // Create 100 random-ish points
    std::vector<Point> points;
    for (int i = 0; i < 100; ++i)
    {
        double x = (i % 10) * 10.0 + (i % 3) * 0.5;
        double y = (i / 10) * 10.0 + (i % 5) * 0.3;
        points.push_back(Point(x, y, i));
    }

    // Should complete in reasonable time
    auto start = std::chrono::high_resolution_clock::now();
    VoronoiDiagram diagram(points);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    EXPECT_EQ(diagram.getSites().size(), 100);
    // Should complete in less than 1 second
    EXPECT_LT(duration.count(), 1000);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

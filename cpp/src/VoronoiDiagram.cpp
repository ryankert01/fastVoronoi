/**
 * @file VoronoiDiagram.cpp
 * @brief Implementation of VoronoiDiagram class
 */

#include "VoronoiDiagram.h"
#include <cmath>
#include <limits>
#include <stdexcept>
#include <map>
#include <set>

namespace fastvoronoi
{

    VoronoiDiagram::VoronoiDiagram() {}

    VoronoiDiagram::VoronoiDiagram(const std::vector<Point> &points)
    {
        build(points);
    }

    void VoronoiDiagram::build(const std::vector<Point> &points)
    {
        if (points.empty())
        {
            throw std::invalid_argument("Cannot build Voronoi diagram from empty point set");
        }

        // Clear previous data
        sites_.clear();
        cells_.clear();
        edges_.clear();
        vertices_.clear();

        // Copy and sort points by x-coordinate
        sites_ = points;
        for (size_t i = 0; i < sites_.size(); ++i)
        {
            sites_[i].id = i;
        }
        std::sort(sites_.begin(), sites_.end());

        // Handle special cases
        if (sites_.size() == 1)
        {
            // Single point - infinite cell
            VoronoiCell cell(sites_[0], 0);
            cells_.push_back(cell);
            return;
        }

        if (sites_.size() == 2)
        {
            // Two points - perpendicular bisector
            computeBaseCase(sites_);
            return;
        }

        // Use divide-and-conquer for larger sets
        divideAndConquer(sites_, 0, sites_.size() - 1);
        buildCells();
    }

    void VoronoiDiagram::divideAndConquer(std::vector<Point> &points, int left, int right)
    {
        int n = right - left + 1;

        // Base case: 2 or 3 points
        if (n <= 3)
        {
            std::vector<Point> subset(points.begin() + left, points.begin() + right + 1);
            computeBaseCase(subset);
            return;
        }

        // Divide
        int mid = left + (right - left) / 2;

        VoronoiDiagram leftDiagram;
        std::vector<Point> leftPoints(points.begin() + left, points.begin() + mid + 1);
        leftDiagram.divideAndConquer(leftPoints, 0, leftPoints.size() - 1);

        VoronoiDiagram rightDiagram;
        std::vector<Point> rightPoints(points.begin() + mid + 1, points.begin() + right + 1);
        rightDiagram.divideAndConquer(rightPoints, 0, rightPoints.size() - 1);

        // Conquer - merge the two diagrams
        merge(leftDiagram, rightDiagram);
    }

    void VoronoiDiagram::computeBaseCase(const std::vector<Point> &points)
    {
        if (points.size() == 2)
        {
            // Create perpendicular bisector
            const Point &p1 = points[0];
            const Point &p2 = points[1];

            Point midpoint((p1.x + p2.x) / 2.0, (p1.y + p2.y) / 2.0);

            // Direction perpendicular to p1-p2
            double dx = p2.x - p1.x;
            double dy = p2.y - p1.y;
            double len = std::sqrt(dx * dx + dy * dy);

            if (len > 1e-9)
            {
                // Perpendicular direction
                double px = -dy / len;
                double py = dx / len;

                // Create edge extending in both directions
                double extension = 10000.0; // Large value for "infinity"
                Point start(midpoint.x - px * extension, midpoint.y - py * extension);
                Point end(midpoint.x + px * extension, midpoint.y + py * extension);

                Edge edge(start, end, 0, 1, true);
                edges_.push_back(edge);

                vertices_.push_back(start);
                vertices_.push_back(end);
            }
        }
        else if (points.size() == 3)
        {
            // Create Voronoi diagram for three points
            const Point &p1 = points[0];
            const Point &p2 = points[1];
            const Point &p3 = points[2];

            // Compute circumcenter
            Point center = circumcenter(p1, p2, p3);
            vertices_.push_back(center);

            // Create edges from circumcenter
            Point mid12((p1.x + p2.x) / 2.0, (p1.y + p2.y) / 2.0);
            Point mid23((p2.x + p3.x) / 2.0, (p2.y + p3.y) / 2.0);
            Point mid31((p3.x + p1.x) / 2.0, (p3.y + p1.y) / 2.0);

            // Create edges extending from center
            double extension = 10000.0;

            // Edge between p1 and p2
            double dx12 = p2.y - p1.y;
            double dy12 = p1.x - p2.x;
            double len12 = std::sqrt(dx12 * dx12 + dy12 * dy12);
            if (len12 > 1e-9)
            {
                dx12 /= len12;
                dy12 /= len12;
                Point end12(center.x + dx12 * extension, center.y + dy12 * extension);
                edges_.push_back(Edge(center, end12, 0, 1, true));
            }

            // Edge between p2 and p3
            double dx23 = p3.y - p2.y;
            double dy23 = p2.x - p3.x;
            double len23 = std::sqrt(dx23 * dx23 + dy23 * dy23);
            if (len23 > 1e-9)
            {
                dx23 /= len23;
                dy23 /= len23;
                Point end23(center.x + dx23 * extension, center.y + dy23 * extension);
                edges_.push_back(Edge(center, end23, 1, 2, true));
            }

            // Edge between p3 and p1
            double dx31 = p1.y - p3.y;
            double dy31 = p3.x - p1.x;
            double len31 = std::sqrt(dx31 * dx31 + dy31 * dy31);
            if (len31 > 1e-9)
            {
                dx31 /= len31;
                dy31 /= len31;
                Point end31(center.x + dx31 * extension, center.y + dy31 * extension);
                edges_.push_back(Edge(center, end31, 2, 0, true));
            }
        }
    }

    Point VoronoiDiagram::circumcenter(const Point &p1, const Point &p2, const Point &p3) const
    {
        double d = 2.0 * (p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y));

        if (std::abs(d) < 1e-9)
        {
            // Collinear points - return midpoint of p1 and p3
            return Point((p1.x + p3.x) / 2.0, (p1.y + p3.y) / 2.0);
        }

        double p1_sq = p1.x * p1.x + p1.y * p1.y;
        double p2_sq = p2.x * p2.x + p2.y * p2.y;
        double p3_sq = p3.x * p3.x + p3.y * p3.y;

        double ux = (p1_sq * (p2.y - p3.y) + p2_sq * (p3.y - p1.y) + p3_sq * (p1.y - p2.y)) / d;
        double uy = (p1_sq * (p3.x - p2.x) + p2_sq * (p1.x - p3.x) + p3_sq * (p2.x - p1.x)) / d;

        return Point(ux, uy);
    }

    void VoronoiDiagram::perpendicularBisector(const Point &p1, const Point &p2,
                                               Point &start, Point &direction) const
    {
        start.x = (p1.x + p2.x) / 2.0;
        start.y = (p1.y + p2.y) / 2.0;

        double dx = p2.x - p1.x;
        double dy = p2.y - p1.y;
        double len = std::sqrt(dx * dx + dy * dy);

        if (len > 1e-9)
        {
            direction.x = -dy / len;
            direction.y = dx / len;
        }
        else
        {
            direction.x = 0.0;
            direction.y = 1.0;
        }
    }

    void VoronoiDiagram::merge(const VoronoiDiagram &leftDiagram, const VoronoiDiagram &rightDiagram)
    {
        // Simplified merge - copy edges from both diagrams
        // Full implementation would compute the merge contour
        edges_.insert(edges_.end(), leftDiagram.edges_.begin(), leftDiagram.edges_.end());
        edges_.insert(edges_.end(), rightDiagram.edges_.begin(), rightDiagram.edges_.end());

        vertices_.insert(vertices_.end(), leftDiagram.vertices_.begin(), leftDiagram.vertices_.end());
        vertices_.insert(vertices_.end(), rightDiagram.vertices_.begin(), rightDiagram.vertices_.end());
    }

    void VoronoiDiagram::buildCells()
    {
        cells_.clear();
        cells_.resize(sites_.size());

        for (size_t i = 0; i < sites_.size(); ++i)
        {
            cells_[i] = VoronoiCell(sites_[i], i);
        }

        // Build cells from edges
        for (const auto &edge : edges_)
        {
            if (edge.leftSite >= 0 && edge.leftSite < static_cast<int>(cells_.size()))
            {
                cells_[edge.leftSite].addVertex(edge.start);
                cells_[edge.leftSite].addVertex(edge.end);
                if (edge.rightSite >= 0)
                {
                    cells_[edge.leftSite].addNeighbor(edge.rightSite);
                }
            }
            if (edge.rightSite >= 0 && edge.rightSite < static_cast<int>(cells_.size()))
            {
                cells_[edge.rightSite].addVertex(edge.start);
                cells_[edge.rightSite].addVertex(edge.end);
                if (edge.leftSite >= 0)
                {
                    cells_[edge.rightSite].addNeighbor(edge.leftSite);
                }
            }
        }
    }

    void VoronoiDiagram::clipToBoundary(const std::vector<Point> &boundary)
    {
        if (boundary.size() < 3)
        {
            return; // Invalid boundary
        }

        // Get bounding box of boundary
        double minX = boundary[0].x, maxX = boundary[0].x;
        double minY = boundary[0].y, maxY = boundary[0].y;

        for (const auto &p : boundary)
        {
            minX = std::min(minX, p.x);
            maxX = std::max(maxX, p.x);
            minY = std::min(minY, p.y);
            maxY = std::max(maxY, p.y);
        }

        // Clip edges to bounding box
        std::vector<Edge> clippedEdges;
        for (const auto &edge : edges_)
        {
            Edge clipped = clipEdge(edge, minX, maxX, minY, maxY);
            clippedEdges.push_back(clipped);
        }
        edges_ = clippedEdges;

        buildCells();
    }

    Edge VoronoiDiagram::clipEdge(const Edge &edge, double minX, double maxX,
                                  double minY, double maxY) const
    {
        // Cohen-Sutherland line clipping algorithm
        Edge clipped = edge;

        // Simplified clipping - clip to bounding box
        clipped.start.x = std::max(minX, std::min(maxX, edge.start.x));
        clipped.start.y = std::max(minY, std::min(maxY, edge.start.y));
        clipped.end.x = std::max(minX, std::min(maxX, edge.end.x));
        clipped.end.y = std::max(minY, std::min(maxY, edge.end.y));

        return clipped;
    }

    int VoronoiDiagram::findNearest(const Point &query) const
    {
        if (sites_.empty())
        {
            return -1;
        }

        int nearestIdx = 0;
        double minDist = sites_[0].distanceSquaredTo(query);

        for (size_t i = 1; i < sites_.size(); ++i)
        {
            double dist = sites_[i].distanceSquaredTo(query);
            if (dist < minDist)
            {
                minDist = dist;
                nearestIdx = i;
            }
        }

        return nearestIdx;
    }

    std::vector<int> VoronoiDiagram::findNearest(const std::vector<Point> &queries) const
    {
        std::vector<int> results;
        results.reserve(queries.size());

        for (const auto &query : queries)
        {
            results.push_back(findNearest(query));
        }

        return results;
    }

    void VoronoiDiagram::getBounds(double &minX, double &maxX, double &minY, double &maxY) const
    {
        if (sites_.empty())
        {
            minX = maxX = minY = maxY = 0.0;
            return;
        }

        minX = maxX = sites_[0].x;
        minY = maxY = sites_[0].y;

        for (const auto &site : sites_)
        {
            minX = std::min(minX, site.x);
            maxX = std::max(maxX, site.x);
            minY = std::min(minY, site.y);
            maxY = std::max(maxY, site.y);
        }
    }

} // namespace fastvoronoi

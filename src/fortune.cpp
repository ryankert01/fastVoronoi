#include "fortune.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <set>

namespace fortune {

// Precision constant for floating point comparisons
constexpr double EPSILON = 1e-10;

// Scale factor for super triangle in Delaunay triangulation
constexpr double SUPER_TRIANGLE_SCALE = 20.0;

/**
 * @brief 2D point structure with optional ID
 */
struct Point2D {
    double x, y;
    int id;

    Point2D(double x_ = 0, double y_ = 0, int id_ = -1) : x(x_), y(y_), id(id_) {}

    /**
     * @brief Compute squared distance to another point
     * @param other The other point
     * @return Squared Euclidean distance
     */
    double distance_sq(const Point2D& other) const {
        const double dx = x - other.x;
        const double dy = y - other.y;
        return dx * dx + dy * dy;
    }

    bool operator<(const Point2D& other) const {
        if (std::abs(x - other.x) > EPSILON)
            return x < other.x;
        return y < other.y;
    }
};

/**
 * @brief Triangle structure for Delaunay triangulation
 */
struct Triangle {
    int p1, p2, p3;
    Point2D circumcenter;
    double circumradius_sq;

    Triangle(int a, int b, int c) : p1(a), p2(b), p3(c), circumradius_sq(0) {}

    /**
     * @brief Check if triangle contains a vertex
     * @param v Vertex index to check
     * @return true if triangle contains the vertex
     */
    bool contains_vertex(int v) const { return p1 == v || p2 == v || p3 == v; }

    /**
     * @brief Check if triangle has a specific edge
     * @param v1 First vertex of edge
     * @param v2 Second vertex of edge
     * @return true if triangle has this edge
     */
    bool has_edge(int v1, int v2) const {
        return (p1 == v1 && p2 == v2) || (p1 == v2 && p2 == v1) || (p2 == v1 && p3 == v2) ||
               (p2 == v2 && p3 == v1) || (p3 == v1 && p1 == v2) || (p3 == v2 && p1 == v1);
    }
};

/**
 * @brief Edge structure for Delaunay triangulation
 */
struct Edge {
    int p1, p2;

    Edge(int a, int b) : p1(std::min(a, b)), p2(std::max(a, b)) {}

    bool operator<(const Edge& other) const {
        if (p1 != other.p1)
            return p1 < other.p1;
        return p2 < other.p2;
    }

    bool operator==(const Edge& other) const { return p1 == other.p1 && p2 == other.p2; }
};

/**
 * @brief Compute circumcenter of a triangle
 * @param a First vertex
 * @param b Second vertex
 * @param c Third vertex
 * @return Circumcenter point (or centroid if points are collinear)
 */
Point2D compute_circumcenter(const Point2D& a, const Point2D& b, const Point2D& c) {
    const double ax = a.x, ay = a.y;
    const double bx = b.x, by = b.y;
    const double cx = c.x, cy = c.y;

    const double d = 2 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));
    if (std::abs(d) < EPSILON) {
        // Collinear points - return centroid
        return Point2D((ax + bx + cx) / 3.0, (ay + by + cy) / 3.0);
    }

    const double ux = ((ax * ax + ay * ay) * (by - cy) + (bx * bx + by * by) * (cy - ay) +
                       (cx * cx + cy * cy) * (ay - by)) /
                      d;
    const double uy = ((ax * ax + ay * ay) * (cx - bx) + (bx * bx + by * by) * (ax - cx) +
                       (cx * cx + cy * cy) * (bx - ax)) /
                      d;

    return Point2D(ux, uy);
}

/**
 * @brief Compute Delaunay triangulation using Bowyer-Watson algorithm
 * @param points Input points with IDs
 * @return Vector of triangles forming the Delaunay triangulation
 *
 * The Bowyer-Watson algorithm:
 * 1. Create a super-triangle that contains all points
 * 2. For each point, find all triangles whose circumcircle contains it
 * 3. Remove those triangles and re-triangulate the resulting polygon
 * 4. Remove triangles connected to the super-triangle
 */
std::vector<Triangle> delaunay_triangulation(const std::vector<Point2D>& points) {
    if (points.size() < 3) {
        return {};
    }

    // Find bounding box
    double min_x = points[0].x, max_x = points[0].x;
    double min_y = points[0].y, max_y = points[0].y;
    for (const auto& p : points) {
        min_x = std::min(min_x, p.x);
        max_x = std::max(max_x, p.x);
        min_y = std::min(min_y, p.y);
        max_y = std::max(max_y, p.y);
    }

    double dx = max_x - min_x;
    double dy = max_y - min_y;
    double delta_max = std::max(dx, dy);
    double mid_x = (min_x + max_x) / 2.0;
    double mid_y = (min_y + max_y) / 2.0;

    // Create super triangle
    std::vector<Point2D> pts = points;
    pts.push_back(Point2D(mid_x - SUPER_TRIANGLE_SCALE * delta_max, mid_y - delta_max, -1));
    pts.push_back(Point2D(mid_x, mid_y + SUPER_TRIANGLE_SCALE * delta_max, -2));
    pts.push_back(Point2D(mid_x + SUPER_TRIANGLE_SCALE * delta_max, mid_y - delta_max, -3));

    int st1 = pts.size() - 3;
    int st2 = pts.size() - 2;
    int st3 = pts.size() - 1;

    std::vector<Triangle> triangles;
    Triangle super_tri(st1, st2, st3);
    super_tri.circumcenter = compute_circumcenter(pts[st1], pts[st2], pts[st3]);
    super_tri.circumradius_sq = super_tri.circumcenter.distance_sq(pts[st1]);
    triangles.push_back(super_tri);

    // Add points one by one
    for (size_t i = 0; i < points.size(); ++i) {
        const Point2D& p = pts[i];
        std::vector<Edge> edges;
        std::vector<Triangle> new_triangles;

        for (const auto& tri : triangles) {
            double dist_sq = tri.circumcenter.distance_sq(p);
            if (dist_sq <= tri.circumradius_sq + EPSILON) {
                // Point is inside circumcircle, add edges
                edges.push_back(Edge(tri.p1, tri.p2));
                edges.push_back(Edge(tri.p2, tri.p3));
                edges.push_back(Edge(tri.p3, tri.p1));
            } else {
                new_triangles.push_back(tri);
            }
        }

        // Remove duplicate edges
        std::sort(edges.begin(), edges.end());
        std::vector<Edge> unique_edges;
        for (size_t j = 0; j < edges.size(); ++j) {
            if (j == edges.size() - 1 || !(edges[j] == edges[j + 1])) {
                if (j == 0 || !(edges[j] == edges[j - 1])) {
                    unique_edges.push_back(edges[j]);
                }
            }
        }

        // Create new triangles
        for (const auto& edge : unique_edges) {
            Triangle new_tri(edge.p1, edge.p2, i);
            new_tri.circumcenter = compute_circumcenter(pts[edge.p1], pts[edge.p2], pts[i]);
            new_tri.circumradius_sq = new_tri.circumcenter.distance_sq(pts[edge.p1]);
            new_triangles.push_back(new_tri);
        }

        triangles = new_triangles;
    }

    // Remove triangles that use super triangle vertices
    std::vector<Triangle> result;
    for (const auto& tri : triangles) {
        if (!tri.contains_vertex(st1) && !tri.contains_vertex(st2) && !tri.contains_vertex(st3)) {
            result.push_back(tri);
        }
    }

    return result;
}

VoronoiDiagram compute_voronoi(const std::vector<std::array<double, 2>>& points) {
    // Input validation
    if (points.empty()) {
        throw std::invalid_argument("Input points vector cannot be empty");
    }

    if (points.size() < 2) {
        throw std::invalid_argument("At least 2 points are required to compute a Voronoi diagram");
    }

    VoronoiDiagram result;
    result.points = points;

    if (points.size() < 3) {
        // Special case: exactly 2 points
        // Create a simple diagram with no vertices but valid structure
        result.point_region.resize(points.size(), 0);
        return result;
    }

    // Convert to Point2D
    std::vector<Point2D> pts;
    pts.reserve(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        pts.emplace_back(points[i][0], points[i][1], static_cast<int>(i));
    }

    // Compute Delaunay triangulation
    std::vector<Triangle> triangles = delaunay_triangulation(pts);

    // Deduplicate vertices (circumcenters)
    std::map<std::pair<double, double>, int> vertex_map;

    for (const auto& tri : triangles) {
        // Round coordinates for deduplication
        double x = std::round(tri.circumcenter.x / EPSILON) * EPSILON;
        double y = std::round(tri.circumcenter.y / EPSILON) * EPSILON;
        auto key = std::make_pair(x, y);

        if (vertex_map.find(key) == vertex_map.end()) {
            vertex_map[key] = result.vertices.size();
            result.vertices.push_back({tri.circumcenter.x, tri.circumcenter.y});
        }
    }

    // Map triangle indices to vertex indices
    std::map<int, int> triangle_to_vertex;
    for (size_t i = 0; i < triangles.size(); ++i) {
        const auto& tri = triangles[i];
        double x = std::round(tri.circumcenter.x / EPSILON) * EPSILON;
        double y = std::round(tri.circumcenter.y / EPSILON) * EPSILON;
        auto key = std::make_pair(x, y);
        triangle_to_vertex[i] = vertex_map[key];
    }

    // Map edges to triangles
    std::map<std::pair<int, int>, std::vector<int>> edge_to_triangles;

    for (size_t i = 0; i < triangles.size(); ++i) {
        const auto& tri = triangles[i];
        std::vector<std::pair<int, int>> edges = {
            {std::min(tri.p1, tri.p2), std::max(tri.p1, tri.p2)},
            {std::min(tri.p2, tri.p3), std::max(tri.p2, tri.p3)},
            {std::min(tri.p3, tri.p1), std::max(tri.p3, tri.p1)}};

        for (const auto& e : edges) {
            edge_to_triangles[e].push_back(i);
        }
    }

    // Build ridge_points and ridge_vertices
    for (const auto& pair : edge_to_triangles) {
        result.ridge_points.push_back({pair.first.first, pair.first.second});

        if (pair.second.size() == 2) {
            // Internal edge
            result.ridge_vertices.push_back(
                {triangle_to_vertex[pair.second[0]], triangle_to_vertex[pair.second[1]]});
        } else if (pair.second.size() == 1) {
            // Boundary edge
            result.ridge_vertices.push_back({triangle_to_vertex[pair.second[0]], -1});
        }
    }

    // Build regions for each point
    std::vector<std::vector<int>> all_regions;
    all_regions.push_back({});  // Empty region at index 0

    result.point_region.resize(points.size());

    for (size_t i = 0; i < points.size(); ++i) {
        std::set<int> region_vertices_set;
        bool has_boundary_edge = false;

        // Find all triangles that contain this point
        for (size_t j = 0; j < triangles.size(); ++j) {
            if (triangles[j].contains_vertex(i)) {
                region_vertices_set.insert(triangle_to_vertex[j]);
            }
        }

        // Check if point is on boundary
        for (const auto& pair : edge_to_triangles) {
            if ((pair.first.first == static_cast<int>(i) ||
                 pair.first.second == static_cast<int>(i)) &&
                pair.second.size() == 1) {
                has_boundary_edge = true;
                break;
            }
        }

        std::vector<int> region_vertices(region_vertices_set.begin(), region_vertices_set.end());

        if (has_boundary_edge) {
            region_vertices.insert(region_vertices.begin(), -1);
        }

        // Find or add region
        int region_idx = -1;
        for (size_t r = 0; r < all_regions.size(); ++r) {
            if (all_regions[r] == region_vertices) {
                region_idx = r;
                break;
            }
        }

        if (region_idx == -1) {
            region_idx = all_regions.size();
            all_regions.push_back(region_vertices);
        }

        result.point_region[i] = region_idx;
    }

    result.regions = all_regions;

    return result;
}

}  // namespace fortune

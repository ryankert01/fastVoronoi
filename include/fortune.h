#ifndef FORTUNE_H
#define FORTUNE_H

#include <algorithm>
#include <array>
#include <cmath>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

namespace fortune {

/**
 * @brief Result structure containing the computed Voronoi diagram
 *
 * This structure contains all the geometric information about a Voronoi diagram,
 * compatible with scipy.spatial.Voronoi output format.
 */
struct VoronoiDiagram {
    std::vector<std::array<double, 2>> points;     ///< Input points
    std::vector<std::array<double, 2>> vertices;   ///< Voronoi vertices (circumcenters)
    std::vector<std::array<int, 2>> ridge_points;  ///< Pairs of point indices forming ridges
    std::vector<std::vector<int>>
        ridge_vertices;                     ///< Vertex indices for each ridge (-1 for infinity)
    std::vector<std::vector<int>> regions;  ///< Vertex indices for each Voronoi region
    std::vector<int> point_region;          ///< Region index for each input point
};

/**
 * @brief Compute Voronoi diagram using Delaunay triangulation approach
 *
 * Uses the Bowyer-Watson algorithm to compute the Delaunay triangulation,
 * then derives the Voronoi diagram as its geometric dual.
 *
 * @param points Input points as 2D coordinates
 * @return VoronoiDiagram containing all geometric information
 * @throws std::invalid_argument if points vector is empty or has fewer than 2 points
 *
 * @note Time complexity: O(n log n) average case, O(n²) worst case
 * @note Space complexity: O(n)
 */
VoronoiDiagram compute_voronoi(const std::vector<std::array<double, 2>>& points);

}  // namespace fortune

#endif  // FORTUNE_H

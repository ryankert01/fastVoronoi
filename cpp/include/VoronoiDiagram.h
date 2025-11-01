/**
 * @file VoronoiDiagram.h
 * @brief Main class for computing and storing Voronoi diagrams
 */

#ifndef FASTVORONOI_VORONOIDIAGRAM_H
#define FASTVORONOI_VORONOIDIAGRAM_H

#include "Point.h"
#include "Edge.h"
#include "VoronoiCell.h"
#include <vector>
#include <memory>
#include <algorithm>

namespace fastvoronoi
{

    /**
     * @brief Main class for Voronoi diagram computation
     *
     * Uses a divide-and-conquer algorithm for efficient computation.
     */
    class VoronoiDiagram
    {
    public:
        /**
         * @brief Default constructor
         */
        VoronoiDiagram();

        /**
         * @brief Constructor with points
         * @param points Vector of generator points
         */
        explicit VoronoiDiagram(const std::vector<Point> &points);

        /**
         * @brief Build the Voronoi diagram from points
         * @param points Vector of generator points
         */
        void build(const std::vector<Point> &points);

        /**
         * @brief Clip the diagram to a bounding polygon
         * @param boundary Vector of points defining the boundary polygon
         */
        void clipToBoundary(const std::vector<Point> &boundary);

        /**
         * @brief Get all cells in the diagram
         * @return Vector of Voronoi cells
         */
        const std::vector<VoronoiCell> &getCells() const { return cells_; }

        /**
         * @brief Get all edges in the diagram
         * @return Vector of edges
         */
        const std::vector<Edge> &getEdges() const { return edges_; }

        /**
         * @brief Get all vertices in the diagram
         * @return Vector of vertices
         */
        const std::vector<Point> &getVertices() const { return vertices_; }

        /**
         * @brief Get the generator points
         * @return Vector of generator points
         */
        const std::vector<Point> &getSites() const { return sites_; }

        /**
         * @brief Find the nearest generator point to a query point
         * @param query Query point
         * @return Index of the nearest generator point
         */
        int findNearest(const Point &query) const;

        /**
         * @brief Find the nearest generator points for multiple query points
         * @param queries Vector of query points
         * @return Vector of indices of nearest generator points
         */
        std::vector<int> findNearest(const std::vector<Point> &queries) const;

        /**
         * @brief Get the bounding box of all sites
         * @param minX Minimum x-coordinate (output)
         * @param maxX Maximum x-coordinate (output)
         * @param minY Minimum y-coordinate (output)
         * @param maxY Maximum y-coordinate (output)
         */
        void getBounds(double &minX, double &maxX, double &minY, double &maxY) const;

    private:
        std::vector<Point> sites_;       ///< Generator points
        std::vector<VoronoiCell> cells_; ///< Voronoi cells
        std::vector<Edge> edges_;        ///< Voronoi edges
        std::vector<Point> vertices_;    ///< Voronoi vertices

        /**
         * @brief Divide-and-conquer algorithm to build Voronoi diagram
         * @param points Sorted vector of generator points
         * @param left Left index
         * @param right Right index
         */
        void divideAndConquer(std::vector<Point> &points, int left, int right);

        /**
         * @brief Merge two Voronoi diagrams
         * @param leftDiagram Left diagram
         * @param rightDiagram Right diagram
         */
        void merge(const VoronoiDiagram &leftDiagram, const VoronoiDiagram &rightDiagram);

        /**
         * @brief Compute Voronoi diagram for 2 or 3 points (base case)
         * @param points Vector of 2 or 3 points
         */
        void computeBaseCase(const std::vector<Point> &points);

        /**
         * @brief Compute the circumcenter of three points
         * @param p1 First point
         * @param p2 Second point
         * @param p3 Third point
         * @return Circumcenter point
         */
        Point circumcenter(const Point &p1, const Point &p2, const Point &p3) const;

        /**
         * @brief Compute the perpendicular bisector of two points
         * @param p1 First point
         * @param p2 Second point
         * @param start Start point of the bisector (output)
         * @param direction Direction vector of the bisector (output)
         */
        void perpendicularBisector(const Point &p1, const Point &p2,
                                   Point &start, Point &direction) const;

        /**
         * @brief Clip an edge to a bounding box
         * @param edge Edge to clip
         * @param minX Minimum x-coordinate of bounding box
         * @param maxX Maximum x-coordinate of bounding box
         * @param minY Minimum y-coordinate of bounding box
         * @param maxY Maximum y-coordinate of bounding box
         * @return Clipped edge
         */
        Edge clipEdge(const Edge &edge, double minX, double maxX,
                      double minY, double maxY) const;

        /**
         * @brief Build cells from edges
         */
        void buildCells();
    };

} // namespace fastvoronoi

#endif // FASTVORONOI_VORONOIDIAGRAM_H

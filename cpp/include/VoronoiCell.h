/**
 * @file VoronoiCell.h
 * @brief Defines a Voronoi cell (region) in the diagram
 */

#ifndef FASTVORONOI_VORONOICELL_H
#define FASTVORONOI_VORONOICELL_H

#include "Point.h"
#include "Edge.h"
#include <vector>

namespace fastvoronoi
{

    /**
     * @brief Represents a Voronoi cell (region) associated with a generator point
     *
     * A cell is the region of space closer to its generator point than to any other.
     */
    class VoronoiCell
    {
    public:
        Point site;                  ///< Generator point for this cell
        int siteIndex;               ///< Index of the generator point
        std::vector<Point> vertices; ///< Vertices of the cell polygon (ordered)
        std::vector<int> neighbors;  ///< Indices of neighboring generator points

        /**
         * @brief Default constructor
         */
        VoronoiCell() : siteIndex(-1) {}

        /**
         * @brief Constructor with site
         * @param s Generator point
         * @param idx Index of the generator point
         */
        VoronoiCell(const Point &s, int idx) : site(s), siteIndex(idx) {}

        /**
         * @brief Add a vertex to the cell
         * @param v Vertex to add
         */
        void addVertex(const Point &v)
        {
            vertices.push_back(v);
        }

        /**
         * @brief Add a neighbor
         * @param neighborIdx Index of the neighboring generator point
         */
        void addNeighbor(int neighborIdx)
        {
            neighbors.push_back(neighborIdx);
        }

        /**
         * @brief Calculate the area of the cell
         * @return Area of the cell polygon
         */
        double area() const
        {
            if (vertices.size() < 3)
                return 0.0;

            double area = 0.0;
            int n = vertices.size();
            for (int i = 0; i < n; ++i)
            {
                int j = (i + 1) % n;
                area += vertices[i].x * vertices[j].y;
                area -= vertices[j].x * vertices[i].y;
            }
            return std::abs(area) / 2.0;
        }

        /**
         * @brief Get the centroid of the cell
         * @return Centroid as a Point
         */
        Point centroid() const
        {
            if (vertices.empty())
                return site;

            double cx = 0.0, cy = 0.0;
            double area = 0.0;
            int n = vertices.size();

            for (int i = 0; i < n; ++i)
            {
                int j = (i + 1) % n;
                double cross = vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y;
                area += cross;
                cx += (vertices[i].x + vertices[j].x) * cross;
                cy += (vertices[i].y + vertices[j].y) * cross;
            }

            area /= 2.0;
            if (std::abs(area) < 1e-9)
                return site;

            cx /= (6.0 * area);
            cy /= (6.0 * area);
            return Point(cx, cy);
        }

        /**
         * @brief Check if a point is inside the cell
         * @param p Point to check
         * @return True if point is inside the cell
         */
        bool contains(const Point &p) const
        {
            if (vertices.size() < 3)
                return false;

            int n = vertices.size();
            bool inside = false;

            for (int i = 0, j = n - 1; i < n; j = i++)
            {
                if (((vertices[i].y > p.y) != (vertices[j].y > p.y)) &&
                    (p.x < (vertices[j].x - vertices[i].x) * (p.y - vertices[i].y) /
                                   (vertices[j].y - vertices[i].y) +
                               vertices[i].x))
                {
                    inside = !inside;
                }
            }

            return inside;
        }
    };

} // namespace fastvoronoi

#endif // FASTVORONOI_VORONOICELL_H

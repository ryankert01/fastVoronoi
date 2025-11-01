/**
 * @file Edge.h
 * @brief Defines edge structures for Voronoi diagram representation
 */

#ifndef FASTVORONOI_EDGE_H
#define FASTVORONOI_EDGE_H

#include "Point.h"
#include <memory>

namespace fastvoronoi
{

    /**
     * @brief Represents an edge in the Voronoi diagram
     *
     * An edge connects two vertices and separates two generator points.
     */
    struct Edge
    {
        Point start;   ///< Starting vertex of the edge
        Point end;     ///< Ending vertex of the edge
        int leftSite;  ///< Index of generator point on the left
        int rightSite; ///< Index of generator point on the right
        bool isRay;    ///< True if this edge extends to infinity

        /**
         * @brief Default constructor
         */
        Edge() : leftSite(-1), rightSite(-1), isRay(false) {}

        /**
         * @brief Constructor with vertices and sites
         * @param s Starting vertex
         * @param e Ending vertex
         * @param left Index of left generator point
         * @param right Index of right generator point
         * @param ray Whether this edge is a ray
         */
        Edge(const Point &s, const Point &e, int left, int right, bool ray = false)
            : start(s), end(e), leftSite(left), rightSite(right), isRay(ray) {}

        /**
         * @brief Get the length of the edge
         * @return Length of the edge
         */
        double length() const
        {
            return start.distanceTo(end);
        }

        /**
         * @brief Get the midpoint of the edge
         * @return Midpoint as a Point
         */
        Point midpoint() const
        {
            return Point((start.x + end.x) / 2.0, (start.y + end.y) / 2.0);
        }
    };

    /**
     * @brief Represents a half-edge in the doubly-connected edge list (DCEL)
     *
     * Half-edges are used for efficient traversal of the Voronoi diagram.
     */
    struct HalfEdge
    {
        Point origin;                   ///< Origin vertex
        std::shared_ptr<HalfEdge> twin; ///< Twin half-edge (opposite direction)
        std::shared_ptr<HalfEdge> next; ///< Next half-edge in the face
        std::shared_ptr<HalfEdge> prev; ///< Previous half-edge in the face
        int incidentSite;               ///< Index of the incident generator point

        /**
         * @brief Default constructor
         */
        HalfEdge() : incidentSite(-1) {}

        /**
         * @brief Constructor with origin
         * @param o Origin vertex
         * @param site Index of incident generator point
         */
        HalfEdge(const Point &o, int site = -1) : origin(o), incidentSite(site) {}
    };

} // namespace fastvoronoi

#endif // FASTVORONOI_EDGE_H

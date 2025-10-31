/**
 * @file Point.h
 * @brief Defines a 2D point structure for Voronoi diagram computation
 */

#ifndef FASTVORONOI_POINT_H
#define FASTVORONOI_POINT_H

#include <cmath>
#include <iostream>

namespace fastvoronoi
{

    /**
     * @brief Represents a 2D point in Euclidean space
     */
    struct Point
    {
        double x; ///< X-coordinate
        double y; ///< Y-coordinate
        int id;   ///< Optional identifier for the point

        /**
         * @brief Default constructor
         */
        Point() : x(0.0), y(0.0), id(-1) {}

        /**
         * @brief Constructor with coordinates
         * @param x_ X-coordinate
         * @param y_ Y-coordinate
         * @param id_ Point identifier (default: -1)
         */
        Point(double x_, double y_, int id_ = -1) : x(x_), y(y_), id(id_) {}

        /**
         * @brief Calculate Euclidean distance to another point
         * @param other The other point
         * @return Distance between this point and other
         */
        double distanceTo(const Point &other) const
        {
            double dx = x - other.x;
            double dy = y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }

        /**
         * @brief Calculate squared distance to another point (faster than distanceTo)
         * @param other The other point
         * @return Squared distance between this point and other
         */
        double distanceSquaredTo(const Point &other) const
        {
            double dx = x - other.x;
            double dy = y - other.y;
            return dx * dx + dy * dy;
        }

        /**
         * @brief Equality operator
         */
        bool operator==(const Point &other) const
        {
            const double EPSILON = 1e-9;
            return std::abs(x - other.x) < EPSILON && std::abs(y - other.y) < EPSILON;
        }

        /**
         * @brief Inequality operator
         */
        bool operator!=(const Point &other) const
        {
            return !(*this == other);
        }

        /**
         * @brief Less-than operator for sorting (by x, then y)
         */
        bool operator<(const Point &other) const
        {
            if (std::abs(x - other.x) > 1e-9)
            {
                return x < other.x;
            }
            return y < other.y;
        }

        /**
         * @brief Stream output operator
         */
        friend std::ostream &operator<<(std::ostream &os, const Point &p)
        {
            os << "(" << p.x << ", " << p.y << ")";
            return os;
        }
    };

} // namespace fastvoronoi

#endif // FASTVORONOI_POINT_H

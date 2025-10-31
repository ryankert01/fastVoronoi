/**
 * @file bindings.cpp
 * @brief Python bindings for fastVoronoi using pybind11
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "Point.h"
#include "Edge.h"
#include "VoronoiCell.h"
#include "VoronoiDiagram.h"

namespace py = pybind11;
using namespace fastvoronoi;

PYBIND11_MODULE(_fastvoronoi, m)
{
    m.doc() = "Fast Voronoi diagram computation library";

    // Point class
    py::class_<Point>(m, "Point")
        .def(py::init<>())
        .def(py::init<double, double>())
        .def(py::init<double, double, int>())
        .def_readwrite("x", &Point::x)
        .def_readwrite("y", &Point::y)
        .def_readwrite("id", &Point::id)
        .def("distance_to", &Point::distanceTo, "Calculate distance to another point")
        .def("distance_squared_to", &Point::distanceSquaredTo, "Calculate squared distance to another point")
        .def("__repr__", [](const Point &p)
             { return "Point(" + std::to_string(p.x) + ", " + std::to_string(p.y) + ")"; })
        .def("__eq__", &Point::operator==)
        .def("__ne__", &Point::operator!=);

    // Edge class
    py::class_<Edge>(m, "Edge")
        .def(py::init<>())
        .def(py::init<const Point &, const Point &, int, int, bool>())
        .def_readwrite("start", &Edge::start)
        .def_readwrite("end", &Edge::end)
        .def_readwrite("left_site", &Edge::leftSite)
        .def_readwrite("right_site", &Edge::rightSite)
        .def_readwrite("is_ray", &Edge::isRay)
        .def("length", &Edge::length, "Get edge length")
        .def("midpoint", &Edge::midpoint, "Get edge midpoint");

    // VoronoiCell class
    py::class_<VoronoiCell>(m, "VoronoiCell")
        .def(py::init<>())
        .def(py::init<const Point &, int>())
        .def_readwrite("site", &VoronoiCell::site)
        .def_readwrite("site_index", &VoronoiCell::siteIndex)
        .def_readwrite("vertices", &VoronoiCell::vertices)
        .def_readwrite("neighbors", &VoronoiCell::neighbors)
        .def("add_vertex", &VoronoiCell::addVertex, "Add a vertex to the cell")
        .def("add_neighbor", &VoronoiCell::addNeighbor, "Add a neighbor")
        .def("area", &VoronoiCell::area, "Calculate cell area")
        .def("centroid", &VoronoiCell::centroid, "Get cell centroid")
        .def("contains", &VoronoiCell::contains, "Check if point is inside cell");

    // VoronoiDiagram class
    py::class_<VoronoiDiagram>(m, "VoronoiDiagram")
        .def(py::init<>())
        .def(py::init<const std::vector<Point> &>())
        .def("build", &VoronoiDiagram::build, "Build Voronoi diagram from points")
        .def("clip_to_boundary", &VoronoiDiagram::clipToBoundary, "Clip diagram to boundary")
        .def("get_cells", &VoronoiDiagram::getCells, "Get all cells")
        .def("get_edges", &VoronoiDiagram::getEdges, "Get all edges")
        .def("get_vertices", &VoronoiDiagram::getVertices, "Get all vertices")
        .def("get_sites", &VoronoiDiagram::getSites, "Get generator points")
        .def("find_nearest", py::overload_cast<const Point &>(&VoronoiDiagram::findNearest, py::const_),
             "Find nearest generator point to query point")
        .def("find_nearest", py::overload_cast<const std::vector<Point> &>(&VoronoiDiagram::findNearest, py::const_),
             "Find nearest generator points for multiple query points")
        .def("get_bounds", [](const VoronoiDiagram &diagram) {
            double minX, maxX, minY, maxY;
            diagram.getBounds(minX, maxX, minY, maxY);
            return py::make_tuple(minX, maxX, minY, maxY);
        }, "Get bounding box of all sites as tuple (minX, maxX, minY, maxY)");

    // Helper function to create VoronoiDiagram from numpy array
    m.def("build_from_array", [](py::array_t<double> points_array)
          {
        auto buf = points_array.request();
        
        if (buf.ndim != 2 || buf.shape[1] != 2) {
            throw std::runtime_error("Input array must be Nx2");
        }
        
        std::vector<Point> points;
        double* ptr = static_cast<double*>(buf.ptr);
        
        for (ssize_t i = 0; i < buf.shape[0]; ++i) {
            double x = ptr[i * 2];
            double y = ptr[i * 2 + 1];
            points.push_back(Point(x, y, i));
        }
        
        return VoronoiDiagram(points); }, "Build Voronoi diagram from numpy array");

    // Helper function to convert points list to numpy array
    m.def("points_to_array", [](const std::vector<Point> &points)
          {
        py::array_t<double> result({static_cast<ssize_t>(points.size()), ssize_t(2)});
        auto buf = result.request();
        double* ptr = static_cast<double*>(buf.ptr);
        
        for (size_t i = 0; i < points.size(); ++i) {
            ptr[i * 2] = points[i].x;
            ptr[i * 2 + 1] = points[i].y;
        }
        
        return result; }, "Convert points list to numpy array");
}

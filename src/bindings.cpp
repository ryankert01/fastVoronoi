#include "fortune.h"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

/**
 * @brief Python wrapper for Voronoi diagram computation
 *
 * Provides a SciPy-compatible interface to the C++ Voronoi implementation.
 */
class Voronoi {
   public:
    /**
     * @brief Construct Voronoi diagram from NumPy array
     * @param points_array NumPy array of shape (N, 2) containing point coordinates
     * @throws std::runtime_error if input is not a valid Nx2 array
     * @throws std::invalid_argument if fewer than 2 points are provided
     */
    Voronoi(py::array_t<double> points_array) {
        auto buf = points_array.request();

        if (buf.ndim != 2 || buf.shape[1] != 2) {
            throw std::runtime_error("Input must be a Nx2 array");
        }

        const size_t n_points = buf.shape[0];
        const double* ptr = static_cast<const double*>(buf.ptr);

        std::vector<std::array<double, 2>> points;
        points.reserve(n_points);

        for (size_t i = 0; i < n_points; ++i) {
            points.push_back({ptr[i * 2], ptr[i * 2 + 1]});
        }

        // Compute Voronoi diagram (may throw std::invalid_argument)
        diagram_ = fortune::compute_voronoi(points);
    }

    py::array_t<double> get_points() const {
        size_t n = diagram_.points.size();
        auto result = py::array_t<double>({n, size_t(2)});
        auto buf = result.request();
        double* ptr = static_cast<double*>(buf.ptr);

        for (size_t i = 0; i < n; ++i) {
            ptr[i * 2] = diagram_.points[i][0];
            ptr[i * 2 + 1] = diagram_.points[i][1];
        }

        return result;
    }

    py::array_t<double> get_vertices() const {
        size_t n = diagram_.vertices.size();
        auto result = py::array_t<double>({n, size_t(2)});
        auto buf = result.request();
        double* ptr = static_cast<double*>(buf.ptr);

        for (size_t i = 0; i < n; ++i) {
            ptr[i * 2] = diagram_.vertices[i][0];
            ptr[i * 2 + 1] = diagram_.vertices[i][1];
        }

        return result;
    }

    py::array_t<int> get_ridge_points() const {
        size_t n = diagram_.ridge_points.size();
        auto result = py::array_t<int>({n, size_t(2)});
        auto buf = result.request();
        int* ptr = static_cast<int*>(buf.ptr);

        for (size_t i = 0; i < n; ++i) {
            ptr[i * 2] = diagram_.ridge_points[i][0];
            ptr[i * 2 + 1] = diagram_.ridge_points[i][1];
        }

        return result;
    }

    py::list get_ridge_vertices() const {
        py::list result;
        for (const auto& ridge : diagram_.ridge_vertices) {
            result.append(ridge);
        }
        return result;
    }

    py::list get_regions() const {
        py::list result;
        for (const auto& region : diagram_.regions) {
            result.append(region);
        }
        return result;
    }

    py::array_t<int> get_point_region() const {
        size_t n = diagram_.point_region.size();
        auto result = py::array_t<int>(n);
        auto buf = result.request();
        int* ptr = static_cast<int*>(buf.ptr);

        for (size_t i = 0; i < n; ++i) {
            ptr[i] = diagram_.point_region[i];
        }

        return result;
    }

    int get_npoints() const { return diagram_.points.size(); }

    int get_ndim() const { return 2; }

   private:
    fortune::VoronoiDiagram diagram_;
};

PYBIND11_MODULE(fortune_voronoi, m) {
    m.doc() = "Fortune's algorithm for computing Voronoi diagrams";

    py::class_<Voronoi>(m, "Voronoi")
        .def(py::init<py::array_t<double>>(), py::arg("points"))
        .def_property_readonly("points", &Voronoi::get_points)
        .def_property_readonly("vertices", &Voronoi::get_vertices)
        .def_property_readonly("ridge_points", &Voronoi::get_ridge_points)
        .def_property_readonly("ridge_vertices", &Voronoi::get_ridge_vertices)
        .def_property_readonly("regions", &Voronoi::get_regions)
        .def_property_readonly("point_region", &Voronoi::get_point_region)
        .def_property_readonly("npoints", &Voronoi::get_npoints)
        .def_property_readonly("ndim", &Voronoi::get_ndim);
}

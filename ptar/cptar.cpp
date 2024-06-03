#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "ParallelTarReader.h"

namespace py = pybind11;

PYBIND11_MODULE(cptar, m) {
    py::class_<ParallelTarReader>(m, "ParallelTarReader")
        .def(py::init<const std::vector<std::string>&, size_t, size_t>())
        .def("get_next_n_data", &ParallelTarReader::get_next_n_data);
}

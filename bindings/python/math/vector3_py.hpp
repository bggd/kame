#pragma once

#include <kame/math/math.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace py = pybind11;

static void initVector3(py::module& m)
{
    using Vector3 = kame::math::Vector3;
    py::class_<Vector3>(m, "Vector3")
        // constructors
        .def(py::init())
        .def(py::init<float>())
        .def(py::init<float, float, float>())
        // member variables
        .def_readwrite("x", &Vector3::x)
        .def_readwrite("y", &Vector3::y)
        .def_readwrite("z", &Vector3::z)
        // operator overloading
        .def(-py::self)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)
        .def(py::self * float())
        .def(py::self / float())
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self *= py::self)
        .def(py::self /= py::self)
        // static methods
        .def("zero", &Vector3::zero)
        .def("one", &Vector3::one)
        .def("dot", &Vector3::dot)
        .def("lengthSquared", &Vector3::lengthSquared)
        .def("length", &Vector3::length)
        .def("normalize", &Vector3::normalize)
        .def("cross", &Vector3::cross)
        .def("transform", [](Vector3 v, const kame::math::Matrix& x) { return Vector3::transform(v, x); })
        .def("transform", [](Vector3 v, kame::math::Quaternion q) { return Vector3::transform(v, q); })
        .def("lerp", &Vector3::lerp);
}

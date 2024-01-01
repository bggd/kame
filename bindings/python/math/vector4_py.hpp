#pragma once

#include <kame/math/math.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace py = pybind11;

static void initVector4(py::module& m)
{
    using Vector4 = kame::math::Vector4;
    py::class_<Vector4>(m, "Vector4")
        // constructors
        .def(py::init())
        .def(py::init<float>())
        .def(py::init<kame::math::Vector3, float>())
        .def(py::init<float, float, float, float>())
        // member variables
        .def_readwrite("x", &Vector4::x)
        .def_readwrite("y", &Vector4::y)
        .def_readwrite("z", &Vector4::z)
        .def_readwrite("w", &Vector4::w)
        // operator overloading
        .def(-py::self)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)
        .def(py::self * float())
        .def(py::self / float())
        // static methods
        .def("zero", &Vector4::zero)
        .def("one", &Vector4::one)
        .def("dot", &Vector4::dot)
        .def("lengthSquared", &Vector4::lengthSquared)
        .def("length", &Vector4::length)
        .def("normalize", &Vector4::normalize)
        .def("transform", &Vector4::transform)
        .def("lerp", &Vector4::lerp);
}

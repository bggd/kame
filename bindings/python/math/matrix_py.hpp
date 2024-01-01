#pragma once

#include <kame/math/math.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace py = pybind11;

static void initMatrix(py::module& m)
{
    using Vector3 = kame::math::Vector3;
    using Vector4 = kame::math::Vector4;
    using Matrix = kame::math::Matrix;
    py::class_<Matrix>(m, "Matrix")
        // constructors
        .def(py::init())
        .def(py::init<Vector4, Vector4, Vector4, Vector4>())
        .def(py::init<float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float>())
        // member variables
        .def_readwrite("m11", &Matrix::m11)
        .def_readwrite("m12", &Matrix::m12)
        .def_readwrite("m13", &Matrix::m13)
        .def_readwrite("m14", &Matrix::m14)
        .def_readwrite("m21", &Matrix::m21)
        .def_readwrite("m22", &Matrix::m22)
        .def_readwrite("m23", &Matrix::m23)
        .def_readwrite("m24", &Matrix::m24)
        .def_readwrite("m31", &Matrix::m31)
        .def_readwrite("m32", &Matrix::m32)
        .def_readwrite("m33", &Matrix::m33)
        .def_readwrite("m34", &Matrix::m34)
        .def_readwrite("m41", &Matrix::m41)
        .def_readwrite("m42", &Matrix::m42)
        .def_readwrite("m43", &Matrix::m43)
        .def_readwrite("m44", &Matrix::m44)
        // operator overloading
        .def(py::self + py::self)
        .def(py::self * py::self)
        .def(py::self * float())
        // methods
        .def("determinant", &Matrix::determinant)
        // static methods
        .def("zero", &Matrix::zero)
        .def("identity", &Matrix::identity)
        .def("multiply", &Matrix::multiply)
        .def("createLookAt", &Matrix::createLookAt)
        .def("createOrthographic_NO", &Matrix::createOrthographic_NO)
        .def("createPerspectiveFieldOfView_NO", &Matrix::createPerspectiveFieldOfView_NO)
        .def("createTranslation", [](Vector3 v) { return Matrix::createTranslation(v); })
        .def("createTranslation", [](float x, float y, float z) { return Matrix::createTranslation(x, y, z); })
        .def("createFromAxisAngle", &Matrix::createFromAxisAngle)
        .def("createRotationX", &Matrix::createRotationX)
        .def("createRotationY", &Matrix::createRotationY)
        .def("createRotationZ", &Matrix::createRotationZ)
        .def("createScale", [](Vector3 v) { return Matrix::createScale(v); })
        .def("createScale", [](float v) { return Matrix::createScale(v); })
        .def("createScale", [](float x, float y, float z) { return Matrix::createScale(x, y, z); })
        .def("createFromQuaternion", &Matrix::createFromQuaternion)
        .def("transpose", &Matrix::transpose)
        .def("invert", &Matrix::invert);
}

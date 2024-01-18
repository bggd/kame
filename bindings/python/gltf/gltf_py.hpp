#pragma once

#include <kame/gltf/gltf.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace py = pybind11;

static void initGltf(py::module& m)
{
    using Gltf = kame::gltf::Gltf;
    py::class_<Gltf>(m, "Gltf")
        .def_readonly("basePath", &Gltf::basePath);

    m.def("loadGltf", &kame::gltf::loadGLTF);
    m.def("loadGLTFFromMemory", &kame::gltf::loadGLTFFromMemory);
    m.def("deleteGLTF", &kame::gltf::deleteGLTF);
}

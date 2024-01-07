#pragma once

#include <kame/squirtle/squirtle.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace py = pybind11;

static void initSquirtle(py::module& m)
{
    m.def("loadFile", &kame::squirtle::loadFile);
    m.def("loadSPIRV", &kame::squirtle::loadSPIRV);

    using Image = kame::squirtle::Image;
    py::class_<Image>(m, "Image")
        .def_readonly("mimeType", &Image::mimeType)
        .def_readonly("url", &Image::url);

    using Texture = kame::squirtle::Texture;
    py::class_<Texture>(m, "Texture")
        .def_readonly("imageIndex", &Texture::imageIndex)
        .def_readonly("magFilter", &Texture::magFilter)
        .def_readonly("minFilter", &Texture::minFilter)
        .def_readonly("wrapS", &Texture::wrapS)
        .def_readonly("wrapT", &Texture::wrapT);

    using Material = kame::squirtle::Material;
    py::class_<Material>(m, "Material")
        .def_readonly("baseColorFactor", &Material::baseColorFactor)
        .def_readonly("baseColorTextureIndex", &Material::baseColorTextureIndex)
        .def_readonly("baseColorTexCoord", &Material::baseColorTexCoord)
        .def_readonly("metallicFactor", &Material::metallicFactor)
        .def_readonly("roughnessFactor", &Material::roughnessFactor)
        .def_readonly("baseColorTextureIndex", &Material::baseColorTextureIndex);

    using AnimationClip = kame::squirtle::AnimationClip;
    py::class_<AnimationClip>(m, "AnimationClip");

    m.def("importAnimation", &kame::squirtle::importAnimation);

    using Model = kame::squirtle::Model;
    py::class_<Model>(m, "Model")
        .def("update", &Model::update);

    m.def("importModel", &kame::squirtle::importModel);
    m.def("animate", &kame::squirtle::animate);

    m.def("importMaterial", &kame::squirtle::importMaterial);
}

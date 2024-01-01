#pragma once

#include <kame/sdl/sdl.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace py = pybind11;

struct PyWindowBase : kame::sdl::WindowBase {
    using kame::sdl::WindowBase::WindowBase;

    void openWindow(const char* title = "kame", int w = 640, int h = 480) override
    {
        PYBIND11_OVERRIDE_PURE(void,
                               kame::sdl::WindowBase,
                               openWindow,
                               title, w, h);
    }

    void closeWindow() override
    {
        PYBIND11_OVERRIDE_PURE(void, kame::sdl::WindowBase, closeWindow, );
    }
};

static void initSDL(py::module& m)
{
    using State = kame::sdl::State;
    py::class_<State>(m, "State")
        .def_readonly("isCloseRequest", &State::isCloseRequest)
        .def_readonly("mouseX", &State::mouseX)
        .def_readonly("mouseY", &State::mouseY)
        .def_readonly("wheelX", &State::wheelX)
        .def_readonly("wheelY", &State::wheelY)
        .def_readonly("isDownLMB", &State::isDownLMB)
        .def_readonly("isDownMMB", &State::isDownMMB)
        .def_readonly("isDownRMB", &State::isDownRMB)
        .def_readonly("isDownX1", &State::isDownX1)
        .def_readonly("isDownX2", &State::isDownX2)
        .def_readonly("isDownScancode", &State::isDownScancode)
        .def_readonly("drawableSizeX", &State::drawableSizeX)
        .def_readonly("drawableSizeY", &State::drawableSizeY);

    using GLVersion = kame::sdl::GLVersion;
    py::class_<GLVersion>(m, "GLVersion")
        .def_readwrite("major", &GLVersion::major)
        .def_readwrite("minor", &GLVersion::minor)
        .def_readwrite("isCore", &GLVersion::isCore);

    using WindowBase = kame::sdl::WindowBase;
    py::class_<WindowBase, PyWindowBase>(m, "WindowBase")
        .def("openWindow", &WindowBase::openWindow)
        .def("closeWindow", &WindowBase::closeWindow)
        .def("setWindowFlags", &WindowBase::setWindowFlags)
        .def("updateInput", &WindowBase::updateInput)
        .def("getState", &WindowBase::getState)
        .def("getElapsedTime", &WindowBase::getElapsedTime)
        .def("delay", &WindowBase::delay);

    using WindowOGL = kame::sdl::WindowOGL;
    py::class_<WindowOGL, WindowBase>(m, "WindowOGL")
        .def("openWindow", &WindowOGL::openWindow)
        .def("closeWindow", &WindowOGL::closeWindow)
        .def("swapWindow", &WindowOGL::swapWindow)
        .def("setOglDebugMode", &WindowOGL::setOglDebugMode)
        .def("setGLVersions", &WindowOGL::setGLVersions)
        .def("setVsync", &WindowOGL::setVsync);

    using WindowVk = kame::sdl::WindowVk;
    py::class_<WindowVk, WindowBase>(m, "WindowVk")
        .def("openWindow", &WindowVk::openWindow)
        .def("closeWindow", &WindowVk::closeWindow);
}

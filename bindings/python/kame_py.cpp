#include <pybind11/pybind11.h>

#include "math/vector3_py.hpp"
#include "math/vector4_py.hpp"
#include "math/matrix_py.hpp"

#include "sdl/sdl_py.hpp"

#include "ogl/ogl_py.hpp"

namespace py = pybind11;

PYBIND11_MODULE(kame, m)
{
    auto math = m.def_submodule("math");
    initVector3(math);
    initVector4(math);
    initMatrix(m);

    auto sdl = m.def_submodule("sdl");
    initSDL(sdl);

    auto ogl = m.def_submodule("ogl");
    initOGL(ogl);
}
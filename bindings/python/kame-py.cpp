#include <pybind11/pybind11.h>

#include "math/vector3_py.hpp"
#include "math/vector4_py.hpp"
#include "math/matrix_py.hpp"

namespace py = pybind11;

PYBIND11_MODULE(kame, m)
{
    auto math = m.def_submodule("math");
    initVector3(math);
    initVector4(math);
    initMatrix(m);
}

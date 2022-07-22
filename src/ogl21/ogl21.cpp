#include <all.hpp>

namespace kame::ogl21 {

void setViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    glViewport(x, y, width, height);
}

void clear(GLbitfield mask)
{
    glClear(mask);
}

void setBlendFunc(GLenum srcRGB, GLenum dstRGB, GLenum srcA, GLenum dstA)
{
    glBlendFuncSeparate(srcRGB, dstRGB, srcA, dstA);
}
} // namespace kame::ogl21
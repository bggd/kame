#pragma once

#include "glad.hpp"

namespace kame::ogl21 {

void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);
void clear(GLbitfield mask);
void setBlendFunc(GLenum srcRGB, GLenum dstRGB, GLenum srcA, GLenum dstA);

} // namespace kame::ogl21
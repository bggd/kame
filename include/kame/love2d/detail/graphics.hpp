#pragma once

#include <kame/ogl/ogl.hpp>

namespace kame::love2d::detail::graphics {

struct Image {
    kame::ogl::Texture2D* tex = nullptr;
    kame::ogl::VertexBuffer* vbo = nullptr;
    kame::ogl::VertexArrayObject vao;

    virtual ~Image();

    bool release();

    int getWidth();
    int getHeight();
};

struct Quad {
    kame::ogl::VertexBuffer* vbo = nullptr;
    kame::ogl::VertexArrayObject vao;

    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int sw = 0;
    int sh = 0;

    virtual ~Quad();

    bool release();
};

} // namespace kame::love2d::detail::graphics

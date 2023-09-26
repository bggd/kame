#pragma once

#include <kame/ogl/ogl.hpp>

namespace kame::love2d::graphics {

struct Image {
    kame::ogl::Texture2D* tex = nullptr;
    kame::ogl::VertexBuffer* vbo = nullptr;
    kame::ogl::VertexArrayObject vao;

    bool release();
};

Image* newImage(const char* filename);

void draw(Image* drawable, float x, float y);

} // namespace kame::love2d::graphics

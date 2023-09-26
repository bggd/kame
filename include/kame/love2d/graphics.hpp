#pragma once

#include <kame/ogl/ogl.hpp>

namespace kame::love2d::graphics {

struct Image {
    kame::ogl::Texture2D* tex = nullptr;
    kame::ogl::VertexBuffer* vbo = nullptr;
    kame::ogl::VertexArrayObject vao;

    bool release();

    int getWidth();
    int getHeight();
};

Image* newImage(const char* filename);

void draw(Image* drawable, float x, float y, float r, float sx, float sy, float ox = 0.0f, float oy = 0.0f);

inline void draw(Image* drawable, float x = 0.0f, float y = 0.0f, float r = 0.0f, float sx = 1.0f) {
    draw(drawable, x, y, r, sx, sx);
}

} // namespace kame::love2d::graphics

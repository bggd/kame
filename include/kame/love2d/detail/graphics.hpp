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
};

Image* newImage(const char* filename);

void draw(Image* drawable, float x, float y, float r, float sx, float sy, float ox = 0.0f, float oy = 0.0f);

inline void draw(Image* drawable, float x = 0.0f, float y = 0.0f, float r = 0.0f, float sx = 1.0f)
{
    draw(drawable, x, y, r, sx, sx);
}

Quad* newQuad(int x, int y, int width, int height, int sw, int sh);

void draw(Image* drawable, Quad* quad, float x, float y, float r, float sx, float sy, float ox = 0.0f, float oy = 0.0f);

inline void draw(Image* drawable, Quad* quad, float x = 0.0f, float y = 0.0f, float r = 0.0f, float sx = 1.0f)
{
    draw(drawable, quad, x, y, r, sx, sx);
}

} // namespace kame::love2d::detail::graphics

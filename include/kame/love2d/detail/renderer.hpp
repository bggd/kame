#pragma once

#include <kame/math/math.hpp>
#include <kame/ogl/ogl.hpp>

namespace kame::love2d::detail::graphics {
struct Image;
struct Quad;
} // namespace kame::love2d::detail::graphics

namespace kame::love2d::detail {

struct Renderer {

    kame::math::Matrix projectionMatrix = kame::math::Matrix::identity();
    kame::ogl::Shader* shaderImageDraw = nullptr;
    kame::ogl::Shader* currentShader = nullptr;
    kame::ogl::Texture2D* currentTexture = nullptr;

    void init();
    void shutdown();
    void preDraw(int32_t drawableSizeX, int32_t drawableSizeY);

    void setShaderImageDraw();
    void setTexture(kame::ogl::Texture2D* tex);

    kame::love2d::detail::graphics::Image* newImage(const char* filename);
    kame::love2d::detail::graphics::Quad* newQuad(int x, int y, int width, int height, int sw, int sh);

    void draw(kame::love2d::detail::graphics::Image* drawable, float x, float y, float r, float sx, float sy, float ox = 0.0f, float oy = 0.0f);
    void draw(kame::love2d::detail::graphics::Image* drawable, kame::love2d::detail::graphics::Quad* quad, float x, float y, float r, float sx, float sy, float ox = 0.0f, float oy = 0.0f);
};

} // namespace kame::love2d::detail

#pragma once

#include <kame/math/math.hpp>
#include <kame/ogl/ogl.hpp>

namespace kame::love2d::detail::graphics {
struct Image;
struct Quad;
} // namespace kame::love2d::detail::graphics

namespace kame::love2d::detail {

struct PolygonVertex {
    float x;
    float y;
};

struct PolygonVBO {
    kame::ogl::VertexBuffer* vbo = nullptr;
    kame::ogl::VertexArrayObject vao;
    size_t numVertex = 256;
    size_t n = 0;
    std::vector<kame::ogl::VertexBuffer*> deleteQueue;

    size_t size() { return n; }
    size_t capacity() { return numVertex; }

    void init();
    void shutdown();

    void clear();
    void reserve(size_t newSize);

    void sendBufferAndDraw(GLenum mode, std::vector<PolygonVertex>& vertices);
};

struct DoubleBufferPolygonVBO {
    std::vector<PolygonVBO> buffers;

    int numBuffer = 2;
    int currentBuffer = 0;

    void initDoubleBufferPolygonVBO(int n = 2);
    void shutdownDoubleBufferPolygonVBO();
    PolygonVBO& getCurrentVBO();
    void flip();
};

struct Renderer {

    kame::math::Vector4 color = kame::math::Vector4::one();
    kame::math::Matrix projectionMatrix = kame::math::Matrix::identity();
    kame::ogl::Shader* shaderImageDraw = nullptr;
    kame::ogl::Shader* shaderPolygonDraw = nullptr;
    kame::ogl::Shader* currentShader = nullptr;
    kame::ogl::Texture2D* currentTexture = nullptr;
    DoubleBufferPolygonVBO polygonBuffer;

    void init();
    void shutdown();
    void preDraw(int32_t drawableSizeX, int32_t drawableSizeY);

    void setShaderImageDraw();
    void setShaderPolygonDraw();
    void setTexture(kame::ogl::Texture2D* tex);

    kame::love2d::detail::graphics::Image* newImage(const char* filename);
    kame::love2d::detail::graphics::Quad* newQuad(int x, int y, int width, int height, int sw, int sh);

    void draw(kame::love2d::detail::graphics::Image* drawable, float x, float y, float r, float sx, float sy, float ox = 0.0f, float oy = 0.0f);
    void draw(kame::love2d::detail::graphics::Image* drawable, kame::love2d::detail::graphics::Quad* quad, float x, float y, float r, float sx, float sy, float ox = 0.0f, float oy = 0.0f);
    void line(std::vector<float>& points);
    void points(std::vector<float>& points);
    void polygon(const char* mode, std::vector<float>& vertices);
    void drawPolygon(GLenum drawMode, const std::vector<float>& vertices);

    void setColor(float red, float green, float blue, float alpha = 1.0f);
    void setColor(std::vector<float>& rgba);
};

} // namespace kame::love2d::detail

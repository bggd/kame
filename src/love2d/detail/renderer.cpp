#include <all.hpp>

const char* imageDrawVertex = R"(#version 330
in vec2 vPos;
in vec2 vUV;
uniform mat4 uMVP;
out vec2 pUV;
void main() {
    pUV = vUV;
    gl_Position = uMVP * vec4(vPos, 0.0, 1.0);
}
)";

const char* imageDrawFragment = R"(#version 330
in vec2 pUV;
uniform sampler2D uAlbedo;
out vec4 fragColor;
void main() {
    fragColor = texture(uAlbedo, pUV);
}
)";

void kame::love2d::detail::Renderer::init()
{
    assert(!shaderImageDraw);

    shaderImageDraw = kame::ogl::createShader(imageDrawVertex, imageDrawFragment);
}

void kame::love2d::detail::Renderer::shutdown()
{
    assert(shaderImageDraw);

    kame::ogl::deleteShader(shaderImageDraw);
    shaderImageDraw = nullptr;
}

void kame::love2d::detail::Renderer::preDraw(int32_t drawableSizeX,
                                             int32_t drawableSizeY)
{
    kame::ogl::setViewport(0, 0, drawableSizeX, drawableSizeY);
    kame::ogl::setClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, kame::math::Vector4(0.5, 0.5, 0.5, 1));
    kame::ogl::RasterizerState rasterState = kame::ogl::RasterizerStateBuilder().build();
    kame::ogl::setRasterizerState(rasterState);
    kame::ogl::BlendState blendState = kame::ogl::BlendStateBuilder()
                                           .blendEquation(GL_FUNC_ADD, GL_FUNC_ADD)
                                           .blendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA)
                                           .build();
    kame::ogl::DepthStencilState depthState = kame::ogl::DepthStencilStateBuilder().build();
    kame::ogl::setBlendState(blendState);
    kame::ogl::setDepthStencilState(depthState);

    projectionMatrix = kame::math::Matrix::createOrthographic_NO(0.0f, drawableSizeX, drawableSizeY, 0.0f, -1.0f, 1.0f);
}

void kame::love2d::detail::Renderer::setShaderImageDraw()
{
    if (currentShader != shaderImageDraw)
    {
        currentShader = shaderImageDraw;
        kame::ogl::setShader(currentShader);
    }
}

void kame::love2d::detail::Renderer::setTexture(kame::ogl::Texture2D* tex)
{
    if (currentTexture != tex)
    {
        currentTexture = tex;
        kame::ogl::setTexture2D(0, currentTexture);
    }
}

kame::love2d::detail::graphics::Image* kame::love2d::detail::Renderer::newImage(const char* filename)
{
    kame::love2d::detail::graphics::Image* img = new kame::love2d::detail::graphics::Image();
    assert(img);
    img->tex = kame::ogl::loadTexture2D(filename);

    float x = 0.0f;
    float y = 0.0f;
    float w = img->tex->width;
    float h = img->tex->height;

    // flip uv.y
    float vtx[] = {
        w, y, 1.0f, 0.0f,
        x, y, 0.0f, 0.0f,
        x, h, 0.0f, 1.0f,
        x, h, 0.0f, 1.0f,
        w, h, 1.0f, 1.0f,
        w, y, 1.0f, 0.0f};

    img->vbo = kame::ogl::createVertexBuffer(sizeof(vtx), GL_STATIC_DRAW);
    img->vbo->setBuffer(vtx);

    img->vao = kame::ogl::VertexArrayObjectBuilder()
                   .bindAttribute(shaderImageDraw->getAttribLocation("vPos"), img->vbo, 2, 4 * sizeof(float), 0)
                   .bindAttribute(shaderImageDraw->getAttribLocation("vUV"), img->vbo, 2, 4 * sizeof(float), 2 * sizeof(float))
                   .build();

    return img;
}

kame::love2d::detail::graphics::Quad* kame::love2d::detail::Renderer::newQuad(int x, int y, int width, int height, int sw, int sh)
{
    kame::love2d::detail::graphics::Quad* quad = new kame::love2d::detail::graphics::Quad();
    assert(quad);

    float sx = 0.0f;
    float sy = 0.0f;
    float w = width;
    float h = height;

    float uvx = float(x) / sw;
    float uvy = float(y) / sh;
    float uvw = w / sw + uvx;
    float uvh = h / sh + uvy;

    // flip uv.y
    // clang-format off
    float vtx[] = {
                    w, sy, uvw, uvy,
                   sx, sy, uvx, uvy,
                   sx,  h, uvx, uvh,
                   sx,  h, uvx, uvh,
                    w,  h, uvw, uvh,
                    w, sy, uvw, uvy};
    // clang-format on

    quad->vbo = kame::ogl::createVertexBuffer(sizeof(vtx), GL_STATIC_DRAW);
    quad->vbo->setBuffer(vtx);

    quad->vao = kame::ogl::VertexArrayObjectBuilder()
                    .bindAttribute(shaderImageDraw->getAttribLocation("vPos"), quad->vbo, 2, 4 * sizeof(float), 0)
                    .bindAttribute(shaderImageDraw->getAttribLocation("vUV"), quad->vbo, 2, 4 * sizeof(float), 2 * sizeof(float))
                    .build();

    return quad;
}

void kame::love2d::detail::Renderer::draw(kame::love2d::detail::graphics::Image* drawable, float x, float y, float r, float sx, float sy, float ox, float oy)
{
    assert(drawable->tex && drawable->vbo);

    kame::math::Matrix T = kame::math::Matrix::createTranslation(x, y, 0.0f);
    kame::math::Matrix R = kame::math::Matrix::createRotationZ(r);
    kame::math::Matrix S = kame::math::Matrix::createScale(sx, sy, 1.0f);
    kame::math::Matrix O = kame::math::Matrix::createTranslation(ox * -1.0f, oy * -1.0f, 0.0f);
    kame::math::Matrix ModelMatrix = O * S * R * T;

    kame::math::Matrix MVP = ModelMatrix * projectionMatrix;
    setShaderImageDraw();
    shaderImageDraw->setMatrix("uMVP", MVP);

    setTexture(drawable->tex);

    drawable->vao.drawArrays(GL_TRIANGLES, 0, 6);
}

void kame::love2d::detail::Renderer::draw(kame::love2d::detail::graphics::Image* drawable, kame::love2d::detail::graphics::Quad* quad, float x, float y, float r, float sx, float sy, float ox, float oy)
{
    assert(drawable->tex && quad->vbo);

    kame::math::Matrix T = kame::math::Matrix::createTranslation(x, y, 0.0f);
    kame::math::Matrix R = kame::math::Matrix::createRotationZ(r);
    kame::math::Matrix S = kame::math::Matrix::createScale(sx, sy, 1.0f);
    kame::math::Matrix O = kame::math::Matrix::createTranslation(ox * -1.0f, oy * -1.0f, 0.0f);
    kame::math::Matrix ModelMatrix = O * S * R * T;

    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    kame::math::Matrix MVP = ModelMatrix * projectionMatrix;
    setShaderImageDraw();
    shaderImageDraw->setMatrix("uMVP", MVP);

    setTexture(drawable->tex);

    quad->vao.drawArrays(GL_TRIANGLES, 0, 6);
}

#include <all.hpp>

void kame::love2d::detail::PolygonVBO::init()
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.win && ctx.renderer);

    ctx.renderer->setShaderPolygonDraw();

    vbo = kame::ogl::createVertexBuffer(sizeof(PolygonVertex) * numVertex, GL_DYNAMIC_DRAW);
    vao.begin()
        .bindAttribute(ctx.renderer->shaderPolygonDraw->getAttribLocation("vPos"), vbo, 2, sizeof(PolygonVertex), 0)
        .end();
}

void kame::love2d::detail::PolygonVBO::shutdown()
{
    for (auto* i : deleteQueue)
    {
        kame::ogl::deleteVertexBuffer(i);
    }
    deleteQueue.clear();
    if (vbo)
    {
        kame::ogl::deleteVertexBuffer(vbo);
        vbo = nullptr;
    }
    vao = kame::ogl::VertexArrayObject();
}

void kame::love2d::detail::PolygonVBO::clear()
{
    n = 0;

    for (auto* i : deleteQueue)
    {
        kame::ogl::deleteVertexBuffer(i);
    }
    deleteQueue.clear();
}

void kame::love2d::detail::PolygonVBO::reserve(size_t newSize)
{
    numVertex = newSize;

    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    SPDLOG_DEBUG("[love2d] PolygonVBO({0}) size: {1}", ctx.renderer->polygonBuffer.currentBuffer, numVertex);

    deleteQueue.push_back(vbo);

    ctx.renderer->setShaderPolygonDraw();

    vbo = kame::ogl::createVertexBuffer(sizeof(PolygonVertex) * numVertex, GL_DYNAMIC_DRAW);
    vao.begin()
        .bindAttribute(ctx.renderer->shaderPolygonDraw->getAttribLocation("vPos"), vbo, 2, sizeof(PolygonVertex), 0)
        .end();

    n = 0;
}

void kame::love2d::detail::PolygonVBO::sendBufferAndDraw(GLenum mode, std::vector<PolygonVertex>& vertices)
{
    vbo->setBufferSubData(this->size() * 2 * sizeof(float), vertices.size() * 2 * sizeof(float), (const float*)vertices.data());
    vao.drawArrays(mode, this->size(), vertices.size());
    this->n += vertices.size();
}

void kame::love2d::detail::DoubleBufferPolygonVBO::initDoubleBufferPolygonVBO(int n)
{
    numBuffer = n;
    for (int i = 0; i < n; ++i)
    {
        buffers.emplace_back();
        buffers.back().init();
    }
}

void kame::love2d::detail::DoubleBufferPolygonVBO::shutdownDoubleBufferPolygonVBO()
{
    for (int i = 0; i < numBuffer; ++i)
    {
        buffers[i].shutdown();
    }
}

kame::love2d::detail::PolygonVBO& kame::love2d::detail::DoubleBufferPolygonVBO::getCurrentVBO()
{
    return buffers[currentBuffer];
}

void kame::love2d::detail::DoubleBufferPolygonVBO::flip()
{
    currentBuffer++;
    if (currentBuffer >= numBuffer)
    {
        currentBuffer = 0;
    }
    buffers[currentBuffer].clear();
}

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

const char* polygonDrawVertex = R"(#version 330
in vec2 vPos;
uniform mat4 uMVP;
void main() {
    gl_Position = uMVP * vec4(vPos, 0.0, 1.0);
}
)";

const char* polygonDrawFragment = R"(#version 330
uniform vec4 uColor;
out vec4 fragColor;
void main() {
    fragColor = uColor;
}
)";

void kame::love2d::detail::Renderer::init()
{
    assert(!shaderImageDraw);
    assert(!shaderPolygonDraw);

    SPDLOG_INFO("Love2D::Renderer init");

    shaderImageDraw = kame::ogl::createShader(imageDrawVertex, imageDrawFragment);

    shaderPolygonDraw = kame::ogl::createShader(polygonDrawVertex, polygonDrawFragment);

    polygonBuffer.initDoubleBufferPolygonVBO();
}

void kame::love2d::detail::Renderer::shutdown()
{
    assert(shaderImageDraw);
    assert(shaderPolygonDraw);

    SPDLOG_INFO("Love2D::Renderer shutdown");

    kame::ogl::deleteShader(shaderImageDraw);
    shaderImageDraw = nullptr;

    kame::ogl::deleteShader(shaderPolygonDraw);
    shaderPolygonDraw = nullptr;

    currentShader = nullptr;

    polygonBuffer.shutdownDoubleBufferPolygonVBO();
}

void kame::love2d::detail::Renderer::preDraw(int32_t drawableSizeX,
                                             int32_t drawableSizeY)
{
    kame::ogl::setViewport(0, 0, drawableSizeX, drawableSizeY);
    kame::ogl::setClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, backgroundColor);
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

    polygonBuffer.flip();
}

void kame::love2d::detail::Renderer::setShaderImageDraw()
{
    if (currentShader != shaderImageDraw)
    {
        currentShader = shaderImageDraw;
        kame::ogl::setShader(currentShader);
    }
}

void kame::love2d::detail::Renderer::setShaderPolygonDraw()
{
    if (currentShader != shaderPolygonDraw)
    {
        currentShader = shaderPolygonDraw;
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

std::tuple<float, float, float, float> kame::love2d::detail::Renderer::getBackgroundColor()
{
    return {backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w};
}

void kame::love2d::detail::Renderer::setBackgroundColor(float red, float green, float blue, float alpha)
{
    backgroundColor = kame::math::Vector4(red, green, blue, alpha);
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

    img->vao.begin()
        .bindAttribute(shaderImageDraw->getAttribLocation("vPos"), img->vbo, 2, 4 * sizeof(float), 0)
        .bindAttribute(shaderImageDraw->getAttribLocation("vUV"), img->vbo, 2, 4 * sizeof(float), 2 * sizeof(float))
        .end();

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

    quad->vao.begin()
        .bindAttribute(shaderImageDraw->getAttribLocation("vPos"), quad->vbo, 2, 4 * sizeof(float), 0)
        .bindAttribute(shaderImageDraw->getAttribLocation("vUV"), quad->vbo, 2, 4 * sizeof(float), 2 * sizeof(float))
        .end();

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

    kame::math::Matrix MVP = ModelMatrix * projectionMatrix;
    setShaderImageDraw();
    shaderImageDraw->setMatrix("uMVP", MVP);

    setTexture(drawable->tex);

    quad->vao.drawArrays(GL_TRIANGLES, 0, 6);
}

void kame::love2d::detail::Renderer::line(std::vector<float>& points)
{
    assert(points.size() >= 4);

    drawPolygon(GL_LINES, points);
}

void kame::love2d::detail::Renderer::points(std::vector<float>& points)
{
    assert(points.size() >= 2);

    drawPolygon(GL_POINTS, points);
}

void kame::love2d::detail::Renderer::polygon(const char* mode, std::vector<float>& vertices)
{
    assert(mode);
    assert(std::string(mode) == "fill" || std::string(mode) == "line");
    GLenum drawMode;
    if (std::string(mode) == "fill")
    {
        drawMode = GL_TRIANGLE_FAN;
    }
    else
    {
        drawMode = GL_LINE_LOOP;
    }

    drawPolygon(drawMode, vertices);
}

void kame::love2d::detail::Renderer::drawPolygon(GLenum drawMode, const std::vector<float>& vertices)
{
    static std::vector<PolygonVertex> polygons;
    polygons.reserve(vertices.size());
    polygons.clear();

    for (size_t i = 0; i < vertices.size(); i += 2)
    {
        PolygonVertex v;
        v.x = *(vertices.begin() + i);
        v.y = *(vertices.begin() + i + 1);
        polygons.emplace_back(v);
    }

    setShaderPolygonDraw();
    shaderPolygonDraw->setMatrix("uMVP", projectionMatrix);
    shaderPolygonDraw->setVector4("uColor", color);

    for (;;)
    {
        if (polygons.size() <= 0)
        {
            break;
        }

        auto& vbo = polygonBuffer.getCurrentVBO();
        size_t rest = vbo.capacity() - vbo.size();

        if (rest >= polygons.size())
        {
            vbo.sendBufferAndDraw(drawMode, polygons);
            break;
        }
        else
        {
            size_t resize = vbo.capacity() * 2;
            for (; resize < polygons.size();)
            {
                resize *= 2;
            }
            vbo.reserve(resize);
            continue;
        }
    }
}

void kame::love2d::detail::Renderer::setColor(float red, float green, float blue, float alpha)
{
    color = kame::math::Vector4(red, green, blue, alpha);
}

void kame::love2d::detail::Renderer::setColor(std::vector<float>& rgba)
{
    assert(rgba.size() >= 3);

    if (rgba.size() == 3)
    {
        rgba.push_back(1.0f);
    }

    setColor(rgba[0], rgba[1], rgba[2], rgba[3]);
}

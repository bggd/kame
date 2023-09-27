#include <all.hpp>

kame::love2d::detail::graphics::Image::~Image()
{
    release();
}

bool kame::love2d::detail::graphics::Image::release()
{
    kame::love2d::Context& ctx = kame::love2d::Context::getInstance();
    if (!ctx.win)
    {
        return false;
    }

    if (tex && vbo)
    {
        kame::ogl::deleteTexture2D(tex);
        tex = nullptr;
        kame::ogl::deleteVertexBuffer(vbo);
        vbo = nullptr;
        return true;
    }
    else
    {
        return false;
    }
}

int kame::love2d::detail::graphics::Image::getWidth()
{
    assert(tex);
    return tex->width;
}

int kame::love2d::detail::graphics::Image::getHeight()
{
    assert(tex);
    return tex->height;
}

kame::love2d::detail::graphics::Image* kame::love2d::detail::graphics::newImage(const char* filename)
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

    kame::love2d::Context& ctx = kame::love2d::Context::getInstance();
    img->vao = kame::ogl::VertexArrayObjectBuilder()
                   .bindAttribute(ctx.shaderImageDraw->getAttribLocation("vPos"), img->vbo, 2, 4 * sizeof(float), 0)
                   .bindAttribute(ctx.shaderImageDraw->getAttribLocation("vUV"), img->vbo, 2, 4 * sizeof(float), 2 * sizeof(float))
                   .build();

    return img;
}

void kame::love2d::detail::graphics::draw(kame::love2d::detail::graphics::Image* drawable, float x, float y, float r, float sx, float sy, float ox, float oy)
{
    assert(drawable->tex && drawable->vbo);

    kame::math::Matrix T = kame::math::Matrix::createTranslation(x, y, 0.0f);
    kame::math::Matrix R = kame::math::Matrix::createRotationZ(r);
    kame::math::Matrix S = kame::math::Matrix::createScale(sx, sy, 1.0f);
    kame::math::Matrix O = kame::math::Matrix::createTranslation(ox * -1.0f, oy * -1.0f, 0.0f);
    kame::math::Matrix ModelMatrix = O * S * R * T;

    kame::love2d::Context& ctx = kame::love2d::Context::getInstance();
    kame::math::Matrix MVP = ModelMatrix * ctx.projectionMatrix;
    ctx.setShaderImageDraw();
    ctx.shaderImageDraw->setMatrix("uMVP", MVP);

    ctx.setTexture(drawable->tex);

    drawable->vao.drawArrays(GL_TRIANGLES, 0, 6);
}

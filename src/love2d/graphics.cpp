#include <all.hpp>

bool kame::love2d::graphics::Image::release()
{
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

kame::love2d::graphics::Image* kame::love2d::graphics::newImage(const char* filename)
{
    kame::love2d::graphics::Image* img = new kame::love2d::graphics::Image();
    assert(img);
    img->tex = kame::ogl::loadTexture2D(filename);

    float x = 0.0f;
    float y = 0.0f;
    float w = img->tex->width;
    float h = img->tex->height;

    float vtx[] = {
        w, y, 1.0f, 1.0f,
        x, y, 0.0f, 1.0f,
        x, h, 0.0f, 0.0f,
        x, h, 0.0f, 0.0f,
        w, h, 1.0f, 0.0f,
        w, y, 1.0f, 1.0f};

    img->vbo = kame::ogl::createVertexBuffer(sizeof(vtx), GL_STATIC_DRAW);
    img->vbo->setBuffer(vtx);

    kame::love2d::Context& ctx = kame::love2d::Context::getInstance();
    img->vao = kame::ogl::VertexArrayObjectBuilder()
                   .bindAttribute(ctx.shaderImageDraw->getAttribLocation("vPos"), img->vbo, 2, 4 * sizeof(float), 0)
                   .bindAttribute(ctx.shaderImageDraw->getAttribLocation("vUV"), img->vbo, 2, 4 * sizeof(float), 2 * sizeof(float))
                   .build();

    return img;
}

void kame::love2d::graphics::draw(kame::love2d::graphics::Image* drawable, float x, float y)
{
    assert(drawable->tex && drawable->vbo);

    kame::math::Matrix T = kame::math::Matrix::createTranslation(x, y, 0.0f);
    kame::math::Matrix ModelMatrix = T;

    kame::love2d::Context& ctx = kame::love2d::Context::getInstance();
    kame::math::Matrix MVP = ModelMatrix * ctx.projectionMatrix;
    ctx.setShaderImageDraw();
    ctx.shaderImageDraw->setMatrix("uMVP", MVP);

    ctx.setTexture(drawable->tex);

    drawable->vao.drawArrays(GL_TRIANGLES, 0, 6);
}

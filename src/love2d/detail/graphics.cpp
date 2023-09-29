#include <all.hpp>

kame::love2d::detail::graphics::Image::~Image()
{
    release();
}

bool kame::love2d::detail::graphics::Image::release()
{
    kame::love2d::Context& ctx = kame::love2d::Context::getInstance();
    if (!ctx.isValid())
    {
        return false;
    }

    if (tex && vbo)
    {
        if (ctx.renderer->currentTexture == tex)
        {
            ctx.renderer->currentTexture = nullptr;
        }

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

kame::love2d::detail::graphics::Quad::~Quad()
{
    release();
}

bool kame::love2d::detail::graphics::Quad::release()
{
    kame::love2d::Context& ctx = kame::love2d::Context::getInstance();
    if (!ctx.isValid())
    {
        return false;
    }

    if (vbo)
    {
        kame::ogl::deleteVertexBuffer(vbo);
        vbo = nullptr;
        return true;
    }
    else
    {
        return false;
    }
}

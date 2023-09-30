#include <all.hpp>

kame::love2d::Image kame::love2d::graphics::newImage(const char* filename)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return kame::love2d::Image(ctx.renderer->newImage(filename));
}

kame::love2d::Quad kame::love2d::graphics::newQuad(int x, int y, int width, int height, int sw, int sh)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return kame::love2d::Quad(ctx.renderer->newQuad(x, y, width, height, sw, sh));
}

void kame::love2d::graphics::draw(kame::love2d::Image drawable, float x, float y, float r, float sx, float sy, float ox, float oy)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    ctx.renderer->draw(drawable.get(), x, y, r, sx, sy, ox, oy);
}

void kame::love2d::graphics::draw(kame::love2d::Image drawable, float x, float y, float r, float sx)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    ctx.renderer->draw(drawable.get(), x, y, r, sx, sx);
}

void kame::love2d::graphics::draw(kame::love2d::Image drawable, kame::love2d::Quad quad, float x, float y, float r, float sx, float sy, float ox, float oy)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    ctx.renderer->draw(drawable.get(), quad.get(), x, y, r, sx, sy, ox, oy);
}

void kame::love2d::graphics::draw(kame::love2d::Image drawable, kame::love2d::Quad quad, float x, float y, float r, float sx)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    ctx.renderer->draw(drawable.get(), quad.get(), x, y, r, sx, sx);
}

void kame::love2d::graphics::polygon(const char* mode, std::initializer_list<float> vertices)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    ctx.renderer->polygon(mode, vertices);
}

void kame::love2d::graphics::polygon(const char* mode, std::vector<float>& vertices)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    ctx.renderer->polygon(mode, vertices);
}

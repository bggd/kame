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

void kame::love2d::graphics::circle(const char* mode, float x, float y, float radius, int segments)
{
    assert(mode);
    assert(std::string(mode) == "fill" || std::string(mode) == "line");

    if (segments <= 0)
    {
        return;
    }

    static std::vector<float> vertices;
    vertices.reserve(segments * 2);
    vertices.clear();
    float angle_step = 2 * M_PI / segments;
    float angle = 0.0;

    for (int i = 0; i < segments; ++i)
    {
        angle += angle_step;
        vertices.emplace_back(x + ::cosf(angle) * radius);
        vertices.emplace_back(y + ::sinf(angle) * radius);
    }

    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    ctx.renderer->polygon(mode, vertices);
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

void kame::love2d::graphics::line(std::vector<float>& points)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    ctx.renderer->line(points);
}

void kame::love2d::graphics::points(std::vector<float>& points)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    ctx.renderer->points(points);
}

void kame::love2d::graphics::polygon(const char* mode, std::vector<float>& vertices)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    ctx.renderer->polygon(mode, vertices);
}

void kame::love2d::graphics::setBackgroundColor(float red, float green, float blue, float alpha)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    ctx.renderer->setBackgroundColor(red, green, blue, alpha);
}

void kame::love2d::graphics::setColor(float red, float green, float blue, float alpha)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    ctx.renderer->setColor(red, green, blue, alpha);
}

void kame::love2d::graphics::setColor(std::vector<float>& rgba)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    ctx.renderer->setColor(rgba);
}

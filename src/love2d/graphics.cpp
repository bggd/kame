#include <all.hpp>

kame::love2d::Image kame::love2d::graphics::newImage(const char* filename)
{
    return kame::love2d::Image(kame::love2d::detail::graphics::newImage(filename));
}

kame::love2d::Quad kame::love2d::graphics::newQuad(int x, int y, int width, int height, int sw, int sh)
{
    return kame::love2d::Quad(kame::love2d::detail::graphics::newQuad(x, y, width, height, sw, sh));
}

void kame::love2d::graphics::draw(kame::love2d::Image drawable, float x, float y, float r, float sx, float sy, float ox, float oy)
{
    kame::love2d::detail::graphics::draw(drawable.get(), x, y, r, sx, sy, ox, oy);
}

void kame::love2d::graphics::draw(kame::love2d::Image drawable, float x, float y, float r, float sx)
{
    draw(drawable, x, y, r, sx, sx);
}

void kame::love2d::graphics::draw(kame::love2d::Image drawable, kame::love2d::Quad quad, float x, float y, float r, float sx, float sy, float ox, float oy)
{
    kame::love2d::detail::graphics::draw(drawable.get(), quad.get(), x, y, r, sx, sy, ox, oy);
}

void kame::love2d::graphics::draw(kame::love2d::Image drawable, kame::love2d::Quad quad, float x, float y, float r, float sx)
{
    kame::love2d::detail::graphics::draw(drawable.get(), quad.get(), x, y, r, sx);
}

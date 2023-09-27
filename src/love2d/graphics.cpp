#include <all.hpp>

kame::love2d::graphics::Image kame::love2d::graphics::newImage(const char* filename)
{
    return Image(kame::love2d::detail::graphics::newImage(filename));
}

void kame::love2d::graphics::draw(kame::love2d::graphics::Image drawable, float x, float y, float r, float sx, float sy, float ox, float oy)
{
    kame::love2d::detail::graphics::draw(drawable.get(), x, y, r, sx, sy, ox, oy);
}

void kame::love2d::graphics::draw(kame::love2d::graphics::Image drawable, float x, float y, float r, float sx)
{
    draw(drawable, x, y, r, sx, sx);
}

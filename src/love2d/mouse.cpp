#include <all.hpp>

int kame::love2d::mouse::getX()
{
    assert(kame::love2d::Context::getInstance().win);
    return kame::love2d::Context::getInstance().win->getState().mouseX;
}

int kame::love2d::mouse::getY()
{
    assert(kame::love2d::Context::getInstance().win);
    return kame::love2d::Context::getInstance().win->getState().mouseY;
}

#include <all.hpp>

float kame::love2d::mouse::getX()
{
    assert(kame::love2d::detail::Context::getInstance().isValid());
    return kame::love2d::detail::Context::getInstance().win->getState().mouseX;
}

float kame::love2d::mouse::getY()
{
    assert(kame::love2d::detail::Context::getInstance().isValid());
    return kame::love2d::detail::Context::getInstance().win->getState().mouseY;
}

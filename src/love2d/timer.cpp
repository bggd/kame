#include <all.hpp>

double kame::love2d::timer::getTime()
{
    assert(kame::love2d::detail::Context::getInstance().isValid());
    return kame::love2d::detail::Context::getInstance().win->getElapsedTime();
}

void kame::love2d::timer::sleep(double s)
{
    assert(kame::love2d::detail::Context::getInstance().isValid());
    kame::love2d::detail::Context::getInstance().win->delay(s);
}

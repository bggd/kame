#include <all.hpp>

double kame::love2d::timer::getTime()
{
    assert(kame::love2d::Context::getInstance().win);
    return kame::love2d::Context::getInstance().win->getElapsedTime();
}

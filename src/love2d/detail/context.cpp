#include <all.hpp>

kame::love2d::detail::Context& kame::love2d::detail::Context::getInstance()
{
    static kame::love2d::detail::Context ctx;
    return ctx;
}

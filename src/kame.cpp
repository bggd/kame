#include <all.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void atexit_handler()
{
    kame::love2d::detail::physics::Physics::destroyQueues();

    SPDLOG_DEBUG("kame::love2d::World: Count {0} / {1}",
                 kame::love2d::detail::physics::Physics::deletedWorldCount,
                 kame::love2d::detail::physics::Physics::createdWorldCount);
    SPDLOG_DEBUG("kame::love2d::Body: Count {0} / {1}",
                 kame::love2d::detail::physics::Physics::deletedBodyCount,
                 kame::love2d::detail::physics::Physics::createdBodyCount);
    SPDLOG_DEBUG("kame::love2d::Fixture Count: {0} / {1}",
                 kame::love2d::detail::physics::Physics::deletedBodyCount,
                 kame::love2d::detail::physics::Physics::createdBodyCount);
}

namespace kame {

void kameInit()
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("kameInit");
    // stbi_set_flip_vertically_on_load(true);
    std::atexit(atexit_handler);
}

void kameShutdown()
{
    spdlog::info("kameShutdown");
}

} // namespace kame

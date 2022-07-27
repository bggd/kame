#include <all.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace kame {

void kameInit()
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("kameInit");
    stbi_set_flip_vertically_on_load(true);
}

void kameShutdown()
{
    spdlog::info("kameShutdown");
}

} // namespace kame

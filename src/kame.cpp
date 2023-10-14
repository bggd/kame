#include <all.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void atexit_handler()
{
}

namespace kame {

bool isInit = false;

void kameInit()
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("kameInit");
    // stbi_set_flip_vertically_on_load(true);
    std::atexit(atexit_handler);
    isInit = true;
}

void kameShutdown()
{
    isInit = false;
    spdlog::info("kameShutdown");
}

bool kameIsInit()
{
    return isInit;
}

} // namespace kame

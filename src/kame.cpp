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
    spdlog::set_pattern("[%H:%M:%S:%e] [%^%l%$] %v");
    SPDLOG_INFO("kameInit");
    // stbi_set_flip_vertically_on_load(true);
    std::atexit(atexit_handler);
    SPDLOG_DEBUG("Exe Path: {}", SDL_GetBasePath());
    assert(!isInit);
    isInit = true;
}

void kameShutdown()
{
    assert(isInit);
    isInit = false;
    SPDLOG_INFO("kameShutdown");
}

bool kameIsInit()
{
    return isInit;
}

} // namespace kame

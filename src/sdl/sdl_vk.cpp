#include <all.hpp>

namespace kame::sdl {

void WindowVk::openWindow(const char* title, int w, int h)
{
#if !defined(NDEBUG)
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
#endif
    SDL_LogSetOutputFunction(spdlogSDL2, NULL);

    // without audio
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS |
             SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_SENSOR);

    window = SDL_CreateWindow(title,
                              w, h,
                              SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN | windowFlags);
    assert(window);

    SDL_ShowWindow(window);

    VkResult r = volkInitialize();
    assert(r == VK_SUCCESS);

    uint32_t version = volkGetInstanceVersion();
    SPDLOG_INFO("Vulkan Version: {}.{}.{}",
                VK_VERSION_MAJOR(version),
                VK_VERSION_MINOR(version),
                VK_VERSION_PATCH(version));
}

void WindowVk::closeWindow()
{
    volkFinalize();
    SDL_DestroyWindow(window);
    SDL_Quit();
}

} // namespace kame::sdl

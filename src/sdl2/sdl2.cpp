#include <all.hpp>

void debugGLMessageCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
void spdlogSDL2(void* userdata, int category, SDL_LogPriority priority, const char* message);

namespace kame::sdl2 {

void Window::openWindow(const char* title, int w, int h)
{
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
    SDL_LogSetOutputFunction(spdlogSDL2, NULL);

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    if (isOGL21DebugMode)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    }

    window = SDL_CreateWindow(title,
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              w, h,
                              SDL_WINDOW_OPENGL);
    assert(window);

    glc = SDL_GL_CreateContext(window);
    assert(glc);
    SDL_GL_MakeCurrent(window, glc);

    int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
    assert(version != 0);
    SPDLOG_INFO("Opengl: {0}.{1}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MAJOR(version));

    assert(GLAD_GL_VERSION_2_1);
    assert(GLAD_GL_EXT_framebuffer_object);

    if (isOGL21DebugMode)
    {
        assert(GLAD_GL_KHR_debug);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debugGLMessageCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    freq = SDL_GetPerformanceFrequency();
    prevTime = SDL_GetPerformanceCounter();
}

void Window::closeWindow()
{
    SDL_GL_DeleteContext(glc);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Window::swapWindow()
{
    SDL_GL_SwapWindow(window);
}

void Window::setOgl21DebugMode(bool debug)
{
    isOGL21DebugMode = debug;
}

void Window::setVsync(bool vsync)
{
    isVsync = vsync;
    if (vsync)
    {
        if (SDL_GL_SetSwapInterval(-1) < 0)
        {
            SDL_GL_SetSwapInterval(1);
        }
    }
    else
    {
        SDL_GL_SetSwapInterval(0);
    }
}

void Window::setFpsCap(double cap)
{
    fpsCap = cap;
}

void Window::update()
{
    uint64_t nowTime = SDL_GetPerformanceCounter();
    double deltaTime = double(nowTime - prevTime) / freq;
    prevTime = nowTime;

    // skip fpsCap < 0
    while (fpsCap > 0.0 && deltaTime < fpsCap)
    {
        uint32_t ms = (uint32_t)((fpsCap - deltaTime) * 1000.0);
        SDL_Delay(ms);
        nowTime = SDL_GetPerformanceCounter();
        deltaTime += double(nowTime - prevTime) / freq;
        prevTime = nowTime;
    }

    state.deltaTime = deltaTime;

    SDL_Event ev;
    while (SDL_PollEvent(&ev) != 0)
    {
        if (ev.type == SDL_QUIT)
        {
            state.isCloseRequest = true;
        }
    }
}

const State& Window::getState()
{
    return state;
}
} // namespace kame::sdl2

void debugGLMessageCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{

    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    {
        return;
    }

    std::string msgSource;
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            msgSource = "GL_DEBUG_SOURCE_API";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            msgSource = "GL_DEBUG_SOURCE_APPLICATION";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            msgSource = "GL_DEBUG_SOURCE_OTHER";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            msgSource = "GL_DEBUG_SOURCE_SHADER_COMPILER";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            msgSource = "GL_DEBUG_SOURCE_THIRD_PARTY";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            msgSource = "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
            break;
        default:
            break;
    }

    std::string msgType;
    switch (type)
    {
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            msgType = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_ERROR:
            msgType = "GL_DEBUG_TYPE_ERROR";
            break;
        case GL_DEBUG_TYPE_OTHER:
            msgType = "GL_DEBUG_TYPE_OTHER";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            msgType = "GL_DEBUG_TYPE_PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            msgType = "GL_DEBUG_TYPE_PORTABILITY";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            msgType = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
            break;
        default:
            break;
    }

    std::string msgSeverity;
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            msgSeverity = "GL_DEBUG_SEVERITY_HIGH";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            msgSeverity = "GL_DEBUG_SEVERITY_LOW";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            msgSeverity = "GL_DEBUG_SEVERITY_MEDIUM";
            break;
        default:
            break;
    }

    SPDLOG_DEBUG("Debug message ({0}): {1}({2},{3},{4})", id, message, msgSource, msgType, msgSeverity);
}

void spdlogSDL2(void* userdata, int category, SDL_LogPriority priority, const char* message)
{
    std::string msg;
    const char* categoryString[] = {
        "SDL_LOG_CATEGORY_APPLICATION",
        "SDL_LOG_CATEGORY_ERROR",
        "SDL_LOG_CATEGORY_ASSERT",
        "SDL_LOG_CATEGORY_SYSTEM",
        "SDL_LOG_CATEGORY_AUDIO",
        "SDL_LOG_CATEGORY_VIDEO",
        "SDL_LOG_CATEGORY_RENDER",
        "SDL_LOG_CATEGORY_INPUT",
        "SDL_LOG_CATEGORY_TEST"};
    if (category <= SDL_LOG_CATEGORY_TEST)
    {
        msg += categoryString[category];
        msg += ": ";
    }
    msg += message;

    switch (priority)
    {
        case SDL_LOG_PRIORITY_VERBOSE:
        case SDL_LOG_PRIORITY_DEBUG:
            SPDLOG_DEBUG(msg);
            break;
        case SDL_LOG_PRIORITY_INFO:
            SPDLOG_INFO(msg);
            break;
        case SDL_LOG_PRIORITY_WARN:
            SPDLOG_WARN(msg);
            break;
        case SDL_LOG_PRIORITY_ERROR:
            SPDLOG_ERROR(msg);
            break;
        case SDL_LOG_PRIORITY_CRITICAL:
        default:
            SPDLOG_CRITICAL(msg);
            break;
    }
}

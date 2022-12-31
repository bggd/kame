#include <all.hpp>

void printSDL_GL_GetAttribute();
void debugGLMessageCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
void spdlogSDL2(void* userdata, int category, SDL_LogPriority priority, const char* message);

namespace kame::sdl {

void WindowOGL::openWindow(const char* title, int w, int h)
{
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
    SDL_LogSetOutputFunction(spdlogSDL2, NULL);

    SDL_Init(SDL_INIT_EVERYTHING);

    std::tuple<int, int> glVersions[] = {{4, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0}, {3, 3}, {3, 2}, {3, 1}, {3, 0}};
    for (std::tuple<int, int> ver : glVersions)
    {
        SDL_GL_ResetAttributes();

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, std::get<0>(ver));
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, std::get<1>(ver));

        if (isOGLDebugMode)
        {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
        }

        window = SDL_CreateWindow(title,
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  w, h,
                                  SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
        if (!window)
        {
            continue;
        }

        glc = SDL_GL_CreateContext(window);
        if (!glc)
        {
            SDL_DestroyWindow(window);
            continue;
        }

        if (glc)
        {
            break;
        }
    }

    // create legacy context
    if (!glc)
    {
        SDL_GL_ResetAttributes();

        if (isOGLDebugMode)
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
    }

    SDL_ShowWindow(window);
    SDL_GL_MakeCurrent(window, glc);
    printSDL_GL_GetAttribute();

    int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
    assert(version != 0);
    SPDLOG_INFO("OpenGL Version: {0}", (const char*)glGetString(GL_VERSION));
    SPDLOG_INFO("OpenGL Renderer: {0}", (const char*)glGetString(GL_RENDERER));
    SPDLOG_INFO("GLSL Version: {0}", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
    kame::ogl::Context::getInstance().versionMajor = GLAD_VERSION_MAJOR(version);
    kame::ogl::Context::getInstance().versionMinor = GLAD_VERSION_MINOR(version);
    int profile = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);
    kame::ogl::Context::getInstance().isCoreProfile = (profile == SDL_GL_CONTEXT_PROFILE_CORE);
    kame::ogl::Context::getInstance().isAvaliable = true;
    SPDLOG_INFO("OpenGL Context: {0}", kame::ogl::Context::getInstance().isCoreProfile ? "Core" : "Compatibility");

    assert(GLAD_GL_VERSION_2_1);
    if (GLAD_VERSION_MAJOR(version) < 3)
    {
        assert(GLAD_GL_ARB_framebuffer_object || GLAD_GL_EXT_framebuffer_object);
    }

    if (isOGLDebugMode)
    {
        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (GLAD_GL_KHR_debug && (flags & GL_CONTEXT_FLAG_DEBUG_BIT))
        {
            SPDLOG_INFO("GL_KHR_debug is avaliable");
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(debugGLMessageCallback, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
        else
        {
            SPDLOG_INFO("GL_KHR_debug is unavaliable");
        }
    }

    if (GLAD_VERSION_MAJOR(version) > 2 && GLAD_GL_ARB_vertex_array_object)
    {
        GLuint VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
    }
    else
    {
        glEnable(GL_TEXTURE_2D);
    }

    freq = SDL_GetPerformanceFrequency();
    prevTime = SDL_GetPerformanceCounter();
}

void WindowOGL::closeWindow()
{
    kame::ogl::Context::getInstance().isAvaliable = false;
    SDL_GL_DeleteContext(glc);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void WindowOGL::swapWindow()
{
    SDL_GL_SwapWindow(window);
}

void WindowOGL::setOgl21DebugMode(bool debug)
{
    isOGLDebugMode = debug;
}

void WindowOGL::setVsync(bool vsync)
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

void WindowOGL::setFpsCap(double cap)
{
    fpsCap = cap;
}

void WindowOGL::update()
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
        switch (ev.type)
        {
            case SDL_QUIT:
                state.isCloseRequest = true;
                break;
            default:
                break;
        }
    }

    auto buttonState = SDL_GetMouseState(&state.mouseX, &state.mouseY);
    state.isDownLMB = state.isDownMMB = state.isDownRMB = state.isDownX1 = state.isDownX2 = false;
    state.isDownLMB = buttonState & SDL_BUTTON_LEFT;
    state.isDownMMB = buttonState & SDL_BUTTON_MIDDLE;
    state.isDownRMB = buttonState & SDL_BUTTON_RIGHT;
    state.isDownX1 = buttonState & SDL_BUTTON_X1;
    state.isDownX2 = buttonState & SDL_BUTTON_X2;
}

const State& WindowOGL::getState()
{
    return state;
}
} // namespace kame::sdl

void printSDL_GL_GetAttribute()
{
    int v;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &v);
    SPDLOG_INFO("SDL_GL_CONTEXT_MAJOR_VERSION: {0}", v);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &v);
    SPDLOG_INFO("SDL_GL_CONTEXT_MINOR_VERSION: {0}", v);
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &v);
    SPDLOG_INFO("SDL_GL_DOUBLEBUFFER: {0}", v);
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &v);
    SPDLOG_INFO("SDL_GL_RED_SIZE: {0}", v);
    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &v);
    SPDLOG_INFO("SDL_GL_GREEN_SIZE: {0}", v);
    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &v);
    SPDLOG_INFO("SDL_GL_BLUE_SIZE: {0}", v);
    SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &v);
    SPDLOG_INFO("SDL_GL_ALPHA_SIZE: {0}", v);
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &v);
    SPDLOG_INFO("SDL_GL_DEPTH_SIZE: {0}", v);
    SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &v);
    SPDLOG_INFO("SDL_GL_BUFFER_SIZE: {0}", v);
}

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

#pragma once

#include "sdl_header.hpp"

#include <vector>
#include <cstdint>

#include "../vk/volk_header.hpp"

namespace kame::sdl {

void spdlogSDL2(void* userdata, int category, SDL_LogPriority priority, const char* message);

struct State {
    bool isCloseRequest = false;
    float mouseX = 0.0f;
    float mouseY = 0.0f;
    int32_t wheelX = 0;
    int32_t wheelY = 0;
    bool isDownLMB = false;
    bool isDownMMB = false;
    bool isDownRMB = false;
    bool isDownX1 = false;
    bool isDownX2 = false;
    bool isDownScancode[SDL_NUM_SCANCODES] = {false};
    int32_t drawableSizeX = 0;
    int32_t drawableSizeY = 0;
};

struct WindowBase {
    SDL_Window* window = nullptr;
    uint32_t windowFlags = 0;
    State state;
    uint64_t elapsedTimeUInt64 = 0;
    double elapsedTime = 0.0;

    virtual void openWindow(const char* title = "kame", int w = 640, int h = 480) = 0;
    virtual void closeWindow() = 0;
    virtual void setWindowFlags(uint32_t flags);
    virtual void updateInput();
    virtual const State& getState();
    virtual double getElapsedTime();
    virtual void delay(double sec);
};

struct GLVersion {
    int major;
    int minor;
    bool isCore;
};

struct WindowOGL : WindowBase {
    SDL_GLContext glc = nullptr;
    std::vector<GLVersion> glVersions;
    bool isOGLDebugMode = false;
    bool isVsync = false;

    virtual void openWindow(const char* title = "kame", int w = 640, int h = 480) override;
    virtual void closeWindow() override;
    void swapWindow();
    void setOglDebugMode(bool debug);
    void setGLVersions(std::vector<GLVersion> versions);
    void setVsync(bool vsync);
};

struct WindowVk : WindowBase {
    virtual void openWindow(const char* title = "kame", int w = 640, int h = 480) override;
    virtual void closeWindow() override;
};

} // namespace kame::sdl

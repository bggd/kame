#pragma once

#include <SDL.h>

#include <cstdint>

namespace kame::sdl {

struct State {
    bool isCloseRequest = false;
    int32_t mouseX = 0;
    int32_t mouseY = 0;
    bool isDownLMB = false;
    bool isDownMMB = false;
    bool isDownRMB = false;
    bool isDownX1 = false;
    bool isDownX2 = false;
    bool isDownScancode[SDL_NUM_SCANCODES] = {false};
    int32_t drawableSizeX = 0;
    int32_t drawableSizeY = 0;
};

struct WindowOGL {
    SDL_Window* window = nullptr;
    SDL_GLContext glc = nullptr;
    bool isOGLDebugMode = false;
    bool isForceGLVersion = false;
    bool isForceCore = false;
    bool isVsync = false;
    int forceMajor = 0, forceMinor;
    State state;
    uint64_t elapsedTimeUInt64 = 0;
    double elapsedTime = 0.0;

    void openWindow(const char* title = "kame", int w = 640, int h = 480);
    void closeWindow();
    void swapWindow();
    void setOglDebugMode(bool debug);
    void forceGLVersion(int majorVersion, int minorVersion, bool core);
    void setVsync(bool vsync);
    void updateInput();
    const State& getState();
    double getElapsedTime();
};

} // namespace kame::sdl

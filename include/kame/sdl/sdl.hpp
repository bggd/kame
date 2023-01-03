#pragma once

#include <SDL.h>

#include <cstdint>

namespace kame::sdl {

struct State {
    bool isCloseRequest = false;
    double deltaTime = 0.0;
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
    bool isVsync = false;
    double fpsCap = -1.0;
    uint64_t freq = 0;
    uint64_t prevTime = 0;
    State state;

    void openWindow(const char* title = "kame", int w = 640, int h = 480);
    void closeWindow();
    void swapWindow();
    void setOglDebugMode(bool debug);
    void setVsync(bool vsync);
    void setFpsCap(double cap);
    void update();
    const State& getState();
};

} // namespace kame::sdl

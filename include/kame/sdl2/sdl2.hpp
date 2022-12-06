#pragma once

#include <SDL.h>

namespace kame::sdl2 {

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
};

struct Window {
    SDL_Window* window = nullptr;
    SDL_GLContext glc = nullptr;
    bool isOGL21DebugMode = false;
    bool isVsync = false;
    double fpsCap = -1.0;
    uint64_t freq = 0;
    uint64_t prevTime = 0;
    State state;

    void openWindow(const char* title = "kame", int w = 640, int h = 480);
    void closeWindow();
    void swapWindow();
    void setOgl21DebugMode(bool debug);
    void setVsync(bool vsync);
    void setFpsCap(double cap);
    void update();
    const State& getState();
};

} // namespace kame::sdl2

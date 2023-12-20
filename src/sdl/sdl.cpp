#include <all.hpp>

namespace kame::sdl {

void WindowBase::setWindowFlags(uint32_t flags)
{
    windowFlags = flags;
}

void WindowBase::updateInput()
{
    state.wheelX = 0;
    state.wheelY = 0;

    SDL_Event ev;
    while (SDL_PollEvent(&ev) != 0)
    {
        switch (ev.type)
        {
            case SDL_EVENT_QUIT:
                state.isCloseRequest = true;
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                state.wheelX = ev.wheel.x;
                state.wheelY = ev.wheel.y;
                if (ev.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                {
                    state.wheelX *= -1;
                    state.wheelY *= -1;
                }
                break;
            default:
                break;
        }
    }

    auto buttonState = SDL_GetMouseState(&state.mouseX, &state.mouseY);
    state.isDownLMB = state.isDownMMB = state.isDownRMB = state.isDownX1 = state.isDownX2 = false;
    state.isDownLMB = buttonState & SDL_BUTTON_LMASK;
    state.isDownMMB = buttonState & SDL_BUTTON_MMASK;
    state.isDownRMB = buttonState & SDL_BUTTON_RMASK;
    state.isDownX1 = buttonState & SDL_BUTTON_X1MASK;
    state.isDownX2 = buttonState & SDL_BUTTON_X2MASK;

    int numKeys;
    auto* scancodeState = SDL_GetKeyboardState(&numKeys);
    assert(numKeys <= SDL_NUM_SCANCODES);
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i)
    {
        state.isDownScancode[i] = false;
    }
    for (int i = 0; i < numKeys; ++i)
    {
        state.isDownScancode[i] = scancodeState[i] == 1;
    }

    SDL_GetWindowSizeInPixels(window, &state.drawableSizeX, &state.drawableSizeY);
}

const State& WindowBase::getState()
{
    return state;
}

double WindowBase::getElapsedTime()
{
    uint64_t now = SDL_GetPerformanceCounter();
    if (now < elapsedTimeUInt64)
    {
        elapsedTimeUInt64 = std::numeric_limits<uint64_t>::max() - elapsedTimeUInt64;
    }
    double x = double(now - elapsedTimeUInt64) / SDL_GetPerformanceFrequency();
    elapsedTimeUInt64 = now;
    elapsedTime += x;
    return elapsedTime;
}

void WindowBase::delay(double sec)
{
    if (sec >= 0)
    {
        SDL_Delay((Uint32)(sec * 1000.0));
    }
}

} // namespace kame::sdl

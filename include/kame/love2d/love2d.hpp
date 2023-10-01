#pragma once

#include <kame/ogl/ogl.hpp>
#include <kame/sdl/sdl.hpp>

#include "detail/renderer.hpp"

namespace kame::love2d {

struct App {
    virtual void load() {}
    virtual void update(float dt) {}
    virtual void draw() {}
};

struct Config {
    struct Window {
        const char* title = "";
        int width = 800;
        int height = 600;
    };

    Window window;
};

void run(App& app, Config& conf);

} // namespace kame::love2d

#include "types.hpp"
#include "detail/graphics.hpp"
#include "detail/context.hpp"
#include "detail/physics.hpp"
#include "graphics.hpp"
#include "physics.hpp"
#include "timer.hpp"
#include "mouse.hpp"

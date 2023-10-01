#pragma once

#include <kame/sdl/sdl.hpp>

#include "renderer.hpp"
#include "physics.hpp"

namespace kame::love2d::detail {

struct Context {

    kame::sdl::WindowOGL* win = nullptr;

    kame::love2d::detail::Renderer* renderer = nullptr;
    kame::love2d::detail::physics::Physics* physics = nullptr;

    bool isValid() { return win && renderer && physics; }

private:
    Context() {}

public:
    static Context& getInstance();
    Context(const Context&) = delete;
    void operator=(const Context&) = delete;
};

} // namespace kame::love2d::detail

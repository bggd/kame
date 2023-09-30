#pragma once

#include <kame/sdl/sdl.hpp>

#include "renderer.hpp"

namespace kame::love2d::detail {

struct Context {

    kame::sdl::WindowOGL* win = nullptr;

    kame::love2d::detail::Renderer* renderer = nullptr;

    bool isValid() { return win && renderer; }

private:
    Context() {}

public:
    static Context& getInstance();
    Context(const Context&) = delete;
    void operator=(const Context&) = delete;
};

} // namespace kame::love2d::detail

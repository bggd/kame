#pragma once

#include <kame/ogl/ogl.hpp>

namespace kame::love2d {

struct App {
    virtual void onLoad() = 0;
    virtual void onUpdate(float dt) = 0;
    virtual void onDraw() = 0;
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

struct Context {

    kame::math::Matrix projectionMatrix;
    kame::ogl::Shader* shaderImageDraw = nullptr;
    kame::ogl::Shader* currentShader = nullptr;
    kame::ogl::Texture2D* currentTexture = nullptr;

    void setShaderImageDraw();
    void setTexture(kame::ogl::Texture2D* tex);

private:
    Context() {}

public:
    static Context& getInstance();
    Context(const Context&) = delete;
    void operator=(const Context&) = delete;
};

} // namespace kame::love2d

#include "graphics.hpp"

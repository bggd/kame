#include <all.hpp>

const char* imageDrawVertex = R"(#version 330
in vec2 vPos;
in vec2 vUV;
uniform mat4 uMVP;
uniform mat4 uProj;
out vec2 pUV;
void main() {
    pUV = vUV;
    gl_Position = uMVP * vec4(vPos, 0.0, 1.0);
}
)";

const char* imageDrawFragment = R"(#version 330
in vec2 pUV;
uniform sampler2D albedo;
out vec4 fragColor;
void main() {
    fragColor = texture(albedo, pUV);
}
)";

void kame::love2d::run(kame::love2d::App& app, kame::love2d::Config& conf)
{
    kame::kameInit();

    kame::sdl::WindowOGL win;
    win.setOglDebugMode(true);
    win.setGLVersions({{3, 3, false}});
    win.openWindow(conf.window.title, conf.window.width, conf.window.height);
    win.setVsync(true);

    kame::love2d::Context& ctx = kame::love2d::Context::getInstance();
    ctx.win = &win;

    ctx.shaderImageDraw = kame::ogl::createShader(imageDrawVertex, imageDrawFragment);

    app.onLoad();

    double prevTime = win.getElapsedTime();

    for (;;)
    {
        win.updateInput();
        auto state = win.getState();
        if (state.isCloseRequest)
            break;

        double nowTime = win.getElapsedTime();
        app.onUpdate(prevTime - nowTime);
        prevTime = nowTime;

        kame::ogl::setViewport(0, 0, state.drawableSizeX, state.drawableSizeY);
        kame::ogl::setClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, kame::math::Vector4(0.5, 0.5, 0.5, 1));
        kame::ogl::RasterizerState rasterState = kame::ogl::RasterizerStateBuilder().build();
        kame::ogl::setRasterizerState(rasterState);
        kame::ogl::BlendState blendState = kame::ogl::BlendStateBuilder()
                                               .blendEquation(GL_FUNC_ADD, GL_FUNC_ADD)
                                               .blendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA)
                                               .build();
        kame::ogl::DepthStencilState depthState = kame::ogl::DepthStencilStateBuilder().build();
        kame::ogl::setBlendState(blendState);
        kame::ogl::setDepthStencilState(depthState);

        ctx.projectionMatrix = kame::math::Matrix::createOrthographic_NO(0.0f, state.drawableSizeX, state.drawableSizeY, 0.0f, -1.0f, 1.0f);

        app.onDraw();

        win.swapWindow();
    }

    ctx.win = nullptr;

    win.closeWindow();

    kame::kameShutdown();
}

kame::love2d::Context& kame::love2d::Context::getInstance()
{
    static kame::love2d::Context ctx;
    return ctx;
}

void kame::love2d::Context::setShaderImageDraw()
{
    if (currentShader != shaderImageDraw)
    {
        currentShader = shaderImageDraw;
        kame::ogl::setShader(currentShader);
    }
}

void kame::love2d::Context::setTexture(kame::ogl::Texture2D* tex)
{
    if (currentTexture != tex)
    {
        currentTexture = tex;
        kame::ogl::setTexture2D(0, currentTexture);
    }
}

double kame::love2d::timer::getTime()
{
    assert(kame::love2d::Context::getInstance().win);
    return kame::love2d::Context::getInstance().win->getElapsedTime();
}

int kame::love2d::mouse::getX()
{
    assert(kame::love2d::Context::getInstance().win);
    return kame::love2d::Context::getInstance().win->getState().mouseX;
}

int kame::love2d::mouse::getY()
{
    assert(kame::love2d::Context::getInstance().win);
    return kame::love2d::Context::getInstance().win->getState().mouseY;
}

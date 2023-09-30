#include <all.hpp>

void kame::love2d::run(kame::love2d::App& app, kame::love2d::Config& conf)
{
    kame::kameInit();

    kame::sdl::WindowOGL win;
    win.setOglDebugMode(true);
    win.setGLVersions({{3, 3, false}});
    win.openWindow(conf.window.title, conf.window.width, conf.window.height);
    win.setVsync(true);

    SPDLOG_INFO("Love2D init");
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    ctx.win = &win;

    ctx.renderer = new kame::love2d::detail::Renderer();
    assert(ctx.renderer);
    ctx.renderer->init();

    app.load();

    double prevTime = win.getElapsedTime();

    for (;;)
    {
        win.updateInput();
        auto state = win.getState();
        if (state.isCloseRequest)
            break;

        double nowTime = win.getElapsedTime();
        app.update(prevTime - nowTime);
        prevTime = nowTime;

        ctx.renderer->preDraw(state.drawableSizeX, state.drawableSizeY);

        app.draw();

        win.swapWindow();
    }

    SPDLOG_INFO("Love2D shutdown");

    ctx.renderer->shutdown();

    ctx.renderer = nullptr;
    ctx.win = nullptr;

    win.closeWindow();

    kame::kameShutdown();
}

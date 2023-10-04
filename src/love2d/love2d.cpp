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

    SPDLOG_INFO("Love2D::Physics init");
    ctx.physics = new kame::love2d::detail::physics::Physics();
    assert(ctx.physics);

    app.load();

    double prevTime = win.getElapsedTime();

    for (;;)
    {
        win.updateInput();
        auto state = win.getState();
        if (state.isCloseRequest)
            break;

        double nowTime = win.getElapsedTime();
        app.update(nowTime - prevTime);
        prevTime = nowTime;

        ctx.renderer->preDraw(state.drawableSizeX, state.drawableSizeY);

        app.draw();

        win.swapWindow();

        win.delay(0.001);
    }

    ctx.physics->destroyQueues();
    delete ctx.physics;
    ctx.physics = nullptr;

    ctx.renderer->shutdown();

    delete ctx.renderer;
    ctx.renderer = nullptr;
    ctx.win = nullptr;

    win.closeWindow();

    SPDLOG_INFO("Love2D shutdown");

    kame::kameShutdown();
}

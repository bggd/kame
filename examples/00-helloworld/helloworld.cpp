#include <kame/kame.hpp>

using namespace kame::math;

int main(int argc, char** argv)
{
    kame::kameInit();

    kame::sdl::Window win;
    win.setOgl21DebugMode(true);
    win.setFpsCap(1.0 / 60.0);
    win.openWindow();
    win.setVsync(true);

    for (;;)
    {
        win.update();
        auto state = win.getState();
        if (state.isCloseRequest)
            break;
        kame::ogl21::setClearBuffer(GL_COLOR_BUFFER_BIT, Vector4f(1, 1, 1, 1));
        win.swapWindow();
    }

    win.closeWindow();
    kame::kameShutdown();
    return 0;
}

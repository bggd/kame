#include <iostream>

#include <kame/kame.hpp>


using namespace std;

int main(int artc, char** argv)
{
    kame::kameInit();
    kame::sdl2::Window win;
    win.setOgl21DebugMode(true);
    win.setFpsCap(1.0 / 60.0);
    win.openWindow();
    win.setVsync(true);
    for (;;) {
        win.update();
        auto state = win.getState();
        if (state.isCloseRequest) break;
        //cout << state.deltaTime << endl;
        kame::ogl21::clear(GL_COLOR_BUFFER_BIT);
        win.swapWindow();
    }
    win.closeWindow();
    kame::kameShutdown();
    cout << "Hello World!" << endl;
    return 0;
}

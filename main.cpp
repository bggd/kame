#include <iostream>

#include <kame/kame.hpp>


using namespace std;
using namespace kame::math;

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

    Vector3f(-1, -2, -3) + Vector3f(1, 2, 3);
    Vector3f(-1, -2, -3) - Vector3f(1, 2, 3);
    Vector3f(-1, -2, -3) * Vector3f(1, 2, 3);
    Vector3f(-1, -2, -3) / Vector3f(1, 2, 3);

    Matrix4x4f(Vector4f(1, 0, 0, 0), Vector4f(0, 1, 0, 0), Vector4f(0, 0, 1, 0), Vector4f(0, 0, 0, 1));
    return 0;
}

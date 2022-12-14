#include <kame/kame.hpp>
#include <string>

using namespace kame::math;

const char* vert = R"(
#if __VERSION__ < 130
#define in attribute
#define out varying
#endif

in vec3 aPos;
void main() {
  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";

const char* frag = R"(
#if __VERSION__ < 130
#define FragColor gl_FragColor
#else
out vec4 FragColor;
#endif

void main() {
  FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
)";

int main(int argc, char** argv)
{
    kame::kameInit();

    kame::sdl::Window win;
    win.setOgl21DebugMode(true);
    win.setFpsCap(1.0 / 60.0);
    win.openWindow();
    win.setVsync(true);

    std::string vs = kame::ogl21::getGlslVersionString();
    vs += vert;
    std::string fs = kame::ogl21::getGlslVersionString();
    fs += frag;
    auto* shader = kame::ogl21::createShader(vs.c_str(), fs.c_str());

    const float vtx[] = {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f};

    auto* vbo = kame::ogl21::createVertexBuffer(sizeof(vtx), GL_STATIC_DRAW);
    vbo->setBuffer(vtx);

    auto vao = kame::ogl21::VertexArrayObjectBuilder()
                   .bindAttribute(shader->getAttribLocation("aPos"), vbo, 3, 3 * sizeof(float), 0)
                   .build();

    for (;;)
    {
        win.update();
        auto state = win.getState();
        if (state.isCloseRequest)
            break;
        kame::ogl21::setViewport(0, 0, 640, 480);
        kame::ogl21::setClearBuffer(GL_COLOR_BUFFER_BIT, Vector4f(0, 0, 0, 1));
        kame::ogl21::setShader(shader);
        vao.drawArrays(GL_TRIANGLES, 0, 3);
        win.swapWindow();
    }

    kame::ogl21::deleteVertexBuffer(vbo);
    kame::ogl21::deleteShader(shader);

    win.closeWindow();
    kame::kameShutdown();
    return 0;
}

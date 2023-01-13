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

    kame::sdl::WindowOGL win;
    win.setOglDebugMode(true);
    win.openWindow();
    win.setVsync(true);

    std::string vs = kame::ogl::getGlslVersionString();
    vs += vert;
    std::string fs = kame::ogl::getGlslVersionString();
    fs += frag;
    auto* shader = kame::ogl::createShader(vs.c_str(), fs.c_str());

    const float vtx[] = {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f};

    auto* vbo = kame::ogl::createVertexBuffer(sizeof(vtx), GL_STATIC_DRAW);
    vbo->setBuffer(vtx);

    auto vao = kame::ogl::VertexArrayObjectBuilder()
                   .bindAttribute(shader->getAttribLocation("aPos"), vbo, 3, 3 * sizeof(float), 0)
                   .build();

    for (;;)
    {
        win.updateInput();
        auto state = win.getState();
        if (state.isCloseRequest)
            break;
        kame::ogl::setViewport(0, 0, 640, 480);
        kame::ogl::setClearBuffer(GL_COLOR_BUFFER_BIT, Vector4(0, 0, 0, 1));
        kame::ogl::setShader(shader);
        vao.drawArrays(GL_TRIANGLES, 0, 3);
        win.swapWindow();
    }

    kame::ogl::deleteVertexBuffer(vbo);
    kame::ogl::deleteShader(shader);

    win.closeWindow();
    kame::kameShutdown();
    return 0;
}

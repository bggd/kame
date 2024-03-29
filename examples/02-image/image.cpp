#include <kame/kame.hpp>
#include <cassert>
#include <string>

using namespace kame::math;

const char* vert = R"(
#if __VERSION__ < 130
#define in attribute
#define out varying
#endif

in vec3 aPos;
in vec2 aTexCoord;
out vec2 texCoord;
void main() {
  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
  texCoord = aTexCoord;
}
)";

const char* frag = R"(
#if __VERSION__ < 130
#define in varying
#define FragColor gl_FragColor
#define texture texture2D
#else
out vec4 FragColor;
#endif

in vec2 texCoord;
uniform sampler2D colorTexture;
void main() {
  FragColor = texture(colorTexture, texCoord);
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
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f};

    auto* vbo = kame::ogl::createVertexBuffer(sizeof(vtx), GL_STATIC_DRAW);
    vbo->setBuffer(vtx);

    unsigned char img[] = {
        0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
        0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
        0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x00,
        0x01, 0x73, 0x52, 0x47, 0x42, 0x00, 0xae, 0xce, 0x1c, 0xe9, 0x00, 0x00,
        0x00, 0x26, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0x63, 0xf8, 0xff, 0xff,
        0xff, 0x7f, 0xf3, 0x29, 0x79, 0xff, 0xc9, 0xa5, 0x19, 0x28, 0xd1, 0xfc,
        0xff, 0xff, 0xff, 0xff, 0x0c, 0xa3, 0x2e, 0x18, 0x75, 0xc1, 0xa8, 0x0b,
        0x06, 0x89, 0x0b, 0x00, 0xa6, 0x04, 0x1a, 0x2e, 0x51, 0x31, 0x09, 0xc9,
        0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82};
    unsigned int img_len = 108;

    auto* tex = kame::ogl::loadTexture2DFromMemory(img, img_len);
    tex->setTexParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    tex->setTexParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    for (;;)
    {
        win.updateInput();
        auto state = win.getState();
        if (state.isCloseRequest)
            break;
        kame::ogl::setViewport(0, 0, 640, 480);
        kame::ogl::setClearState(GL_COLOR_BUFFER_BIT, Vector4(0, 0, 0, 1));
        kame::ogl::setShader(shader);
        kame::ogl::setTexture2D(0, tex);
        kame::ogl::VertexArrayObject vao;
        vao.begin()
            .bindAttribute(vbo, shader->getAttribLocation("aPos"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0)
            .bindAttribute(vbo, shader->getAttribLocation("aTexCoord"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 3 * sizeof(float))
            .end();
        vao.drawArrays(GL_TRIANGLES, 0, 6);
        win.swapWindow();
    }

    kame::ogl::deleteTexture2D(tex);
    kame::ogl::deleteVertexBuffer(vbo);
    kame::ogl::deleteShader(shader);

    win.closeWindow();
    kame::kameShutdown();
    return 0;
}

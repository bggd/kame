#include <kame/kame.hpp>
#include <array>
#include <vector>
#include <map>
#include <algorithm>
#include <stack>
#include <set>
#include <cstdio>
#include <cstdint>
#include <cmath>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>

#include "../common/common.hpp"
using namespace kame::squirtle;

namespace fov {
// radians on calc
float V2H(float V, float width, float height)
{
    float H = 2.0f * atanf(tanf(V / 2.0f) * (width / height));
    return H;
}

float H2V(float H, float width, float height)
{
    float V = 2.0f * atanf(tanf(H / 2.0f) * (height / width));
    return V;
}

float aspectRatio(float x, float y)
{
    return tanf(x / 2.0f) / tanf(y / 2.0f);
}

} // namespace fov

namespace camera {
// focial lengh(mm), aperture(mm)
float getFov(float focialLength, float aperture)
{
    return 2.0f * atanf(aperture / (2.0f * focialLength));
}

float getFocialLength(float fieldOfView, float aperture)
{
    float x = fieldOfView / 2.0f;
    return (aperture / 2.0f) * (cosf(x) / sinf(x));
}
} // namespace camera

const char* vertGLSL = R"(#version 330
in vec3 vPos;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
void main() {
    mat4 MVP = uProj * uView * uModel;
    gl_Position = MVP * vec4(vPos, 1.0);
}
)";

const char* fragGLSL = R"(#version 330
out vec4 fragColor;
void main() {
    fragColor = vec4(0.278431, 0.447059, 0.701961, 1.0);
}
)";

const char* drawLinesGLSL = R"(#version 330
out vec4 fragColor;
void main() {
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
)";

kame::ogl::VertexBuffer* gVBO = nullptr;
kame::ogl::IndexBuffer* gIBO = nullptr;
std::vector<kame::math::Vector3> gPositions;
std::vector<unsigned int> gIndices;

using namespace kame::math;
using namespace kame::math::helper;

void drawModel(kame::ogl::Shader* shader, Model* model)
{
    kame::ogl::VertexArrayObject vao = kame::ogl::VertexArrayObjectBuilder()
                                           .bindAttribute(shader->getAttribLocation("vPos"), gVBO, 3, 3 * sizeof(float), 0)
                                           .bindIndexBuffer(gIBO)
                                           .build();
    vao.drawElements(GL_TRIANGLES, gIndices.size(), GL_UNSIGNED_INT);
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "modelview: require *.gltf\n");
        return 1;
    }

    kame::kameInit();

    kame::sdl::WindowOGL win;
    win.setOglDebugMode(true);
    win.setGLVersions({{3, 3, false}});
    win.setWindowFlags(SDL_WINDOW_RESIZABLE);
    win.openWindow("modelview", 1280, 720);
    win.setVsync(true);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsLight();
    SDL_AddEventWatch([](void*, SDL_Event* e) -> int { ImGui_ImplSDL3_ProcessEvent(e); return 0; }, nullptr);
    ImGui_ImplSDL3_InitForOpenGL(win.window, win.glc);
    ImGui_ImplOpenGL3_Init("#version 330");

    kame::gltf::Gltf* gltf = kame::gltf::loadGLTF(argv[1]);
    Model* model = importModel(gltf);
    kame::gltf::deleteGLTF(gltf);

    size_t numPos = 0;
    size_t offset = 0;
    size_t numIndex = 0;
    for (auto& n : model->nodes)
    {
        if (n.meshID < 0)
        {
            continue;
        }

        Mesh& srcMesh = model->meshes[n.meshID];
        numPos += srcMesh.getBytesOfPositions();
        numIndex += srcMesh.getBytesOfIndices();

        gPositions.resize(gPositions.size() + srcMesh.positions.size());
        for (const auto& e : srcMesh.indices)
        {
            gIndices.push_back(offset + e);
        }

        offset += srcMesh.positions.size();
    }

    gVBO = kame::ogl::createVertexBuffer(numPos, GL_STATIC_DRAW);
    gVBO->setBuffer(gPositions);
    gIBO = kame::ogl::createIndexBuffer(numIndex, GL_STATIC_DRAW);
    gIBO->setBuffer(gIndices);

    kame::ogl::Shader* shaderFrontFace = kame::ogl::createShader(vertGLSL, fragGLSL);
    kame::ogl::Shader* shaderDrawLines = kame::ogl::createShader(vertGLSL, drawLinesGLSL);

    // for turntable rotation
    Matrix modelMtx = Matrix::identity();
    int prevMouseX = 0, prevMouseY = 0;
    float sensitivity = 0.4f; // degree
    float rotX = 0.0f, rotY = 0.0f;
    float panX = 0.0f, panY = 0.0f;
    float zoom = 0.0f;

    std::vector<const char*> items;
    items.reserve(model->clips.size());
    for (auto& [_, clip] : model->clips)
    {
        items.emplace_back(clip.name.c_str());
    }
    int itemCurrent = 0;
    int itemSelect = 0;
    if (model->hasAnimation())
    {
        model->setAnimationClip(items[itemCurrent]);
        model->playAnimation();
    }

    double prevTime = win.getElapsedTime();

    for (;;)
    {
        win.updateInput();
        auto state = win.getState();
        if (state.isCloseRequest)
            break;

        int relMouseX = prevMouseX - state.mouseX;
        int relMouseY = prevMouseY - state.mouseY;

        if (state.isDownLMB)
        {
            rotX += sensitivity * float(relMouseY) * -1.0f;
            rotY += sensitivity * float(relMouseX) * -1.0f;
            modelMtx = Matrix::createRotationY(toRadians(rotY)) * Matrix::createRotationX(toRadians(rotX));
            modelMtx = modelMtx * Matrix::createTranslation(panX, panY, 0.0f);
        }

        if (state.wheelY != 0)
        {
            zoom += float(state.wheelY) * -1.0f;
            if (zoom < 0.0f)
            {
                zoom = 0.0f;
            }
        }

        if (state.isDownRMB)
        {
            panX += (1.0f + zoom) * 0.5f * 0.005f * float(relMouseX) * -1.0f;
            panY += (1.0f + zoom) * 0.5f * 0.005f * float(relMouseY);
            modelMtx = Matrix::createRotationY(toRadians(rotY)) * Matrix::createRotationX(toRadians(rotX));
            modelMtx = modelMtx * Matrix::createTranslation(panX, panY, 0.0f);
        }

        prevMouseX = state.mouseX;
        prevMouseY = state.mouseY;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        // ImGui::ShowDemoWindow();

        ImGui::ListBox(fmt::format("{} clips", items.size()).data(), &itemSelect, items.data(), items.size(), 10);
        if (itemCurrent != itemSelect)
        {
            itemCurrent = itemSelect;
            model->setAnimationClip(items[itemCurrent]);
            model->playAnimation();
        }
        model->updateAnimation(win.getElapsedTime() - prevTime);
        prevTime = win.getElapsedTime();

        kame::ogl::setViewport(0, 0, state.drawableSizeX, state.drawableSizeY);
        glDepthMask(GL_TRUE);
        kame::ogl::setClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, Vector4(0.5, 0.5, 0.5, 1));
        kame::ogl::RasterizerState rasterState = kame::ogl::RasterizerStateBuilder().cullFace(GL_BACK).build();
        kame::ogl::setRasterizerState(rasterState);
        kame::ogl::BlendState blendState = kame::ogl::BlendStateBuilder()
                                               .blendEquation(GL_FUNC_ADD, GL_FUNC_ADD)
                                               .blendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA)
                                               .build();
        kame::ogl::DepthStencilState depthState = kame::ogl::DepthStencilStateBuilder()
                                                      .depthFunc(GL_LESS)
                                                      .build();
        kame::ogl::setBlendState(blendState);
        kame::ogl::setDepthStencilState(depthState);

        // hor+
        float targetHorizontalFov = toRadians(90.0f);
        float targetWidth = 1280.0f;
        float targetHeight = 720.0f;
        float verticalFov = fov::H2V(targetHorizontalFov, targetWidth, targetHeight);
        Matrix view = Matrix::createLookAt(Vector3(0.0f, 0.0f, 2.5f + zoom), Vector3::zero(), Vector3(0.0f, 1.0f, 0.0f));
        Matrix proj = Matrix::createPerspectiveFieldOfView_NO(verticalFov, float(state.drawableSizeX) / float(state.drawableSizeY), 1.0f, 1000.0f);

        kame::ogl::setShader(shaderFrontFace);
        shaderFrontFace->setMatrix("uView", view);
        shaderFrontFace->setMatrix("uProj", proj);
        shaderFrontFace->setMatrix("uModel", modelMtx);

        model->prepareDraw(gPositions);
        gVBO->setBuffer(gPositions);
        drawModel(shaderFrontFace, model);

        kame::ogl::setShader(shaderDrawLines);
        shaderDrawLines->setMatrix("uView", view);
        shaderDrawLines->setMatrix("uProj", proj);
        shaderDrawLines->setMatrix("uModel", modelMtx);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDepthMask(GL_FALSE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(0.0f, -40.0f);
        depthState = kame::ogl::DepthStencilStateBuilder()
                         .depthFunc(GL_LEQUAL)
                         .build();
        kame::ogl::setDepthStencilState(depthState);
        drawModel(shaderDrawLines, model);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        win.swapWindow();
    }

    kame::ogl::deleteShader(shaderDrawLines);
    kame::ogl::deleteShader(shaderFrontFace);
    win.closeWindow();
    kame::kameShutdown();
    return 0;
}

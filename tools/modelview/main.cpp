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
#include <filesystem>

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

const char* vertTexGLSL = R"(#version 330
in vec3 vPos;
in vec2 vUV;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
out vec2 pUV;
void main() {
    mat4 MVP = uProj * uView * uModel;
    gl_Position = MVP * vec4(vPos, 1.0);
    pUV = vUV;
}
)";

const char* fragTexGLSL = R"(#version 330
in vec2 pUV;
uniform vec4 uBaseColorFactor;
uniform sampler2D uTex;
out vec4 fragColor;
void main() {
    fragColor = texture(uTex, pUV) * uBaseColorFactor;
}
)";

const char* drawLinesGLSL = R"(#version 330
out vec4 fragColor;
void main() {
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
)";

kame::ogl::VertexBuffer* gVBO = nullptr;
kame::ogl::VertexBuffer* gVBOTexCoord = nullptr;
kame::ogl::IndexBuffer* gIBO = nullptr;
std::vector<kame::math::Vector3> gPositions;
std::vector<kame::math::Vector2> gUV;
std::vector<unsigned int> gIndices;

std::vector<kame::ogl::Texture2D*> gTextures;

kame::ogl::Shader* gShaderFrontFace = nullptr;
kame::ogl::Shader* gShaderTexture = nullptr;
kame::ogl::Shader* gShaderDrawLines = nullptr;
bool isEdgeLines = false;

using namespace kame::math;
using namespace kame::math::helper;

void drawModelWithTexture(const std::vector<kame::math::Vector3>& positions, const kame::squirtle::Model& model, const kame::squirtle::Primitive& pri)
{
    assert(pri.material < model.materials.size());
    kame::squirtle::Material mat = model.materials[pri.material];
    gShaderTexture->setVector4("uBaseColorFactor", mat.baseColorFactor);

    assert(mat.baseColorTextureIndex >= 0 && mat.baseColorTextureIndex < gTextures.size());
    assert(gTextures[mat.baseColorTextureIndex]);
    kame::ogl::setTexture2D(0, gTextures[mat.baseColorTextureIndex]);

    gVBO->setBuffer(positions);

    assert(mat.baseColorTexCoord >= 0 && mat.baseColorTexCoord < pri.uvSets.size());
    auto& uvSet = pri.getUvSets()[mat.baseColorTexCoord];
    std::copy(uvSet.begin(), uvSet.end(), gUV.begin());
    gVBOTexCoord->setBuffer(gUV);

    std::copy(pri.getIndices().begin(), pri.getIndices().end(), gIndices.begin());
    gIBO->setBuffer(gIndices);

    kame::ogl::VertexArrayObject vao;
    vao.begin()
        .bindAttribute(gShaderTexture->getAttribLocation("vPos"), gVBO, 3, 3 * sizeof(float), 0)
        .bindAttribute(gShaderTexture->getAttribLocation("vUV"), gVBOTexCoord, 2, 2 * sizeof(float), 0)
        .bindIndexBuffer(gIBO)
        .end();
    vao.drawElements(pri.mode, pri.getIndices().size(), GL_UNSIGNED_INT);
}

void drawModel(const std::vector<kame::math::Vector3>& positions, const kame::squirtle::Model& model, const kame::squirtle::Primitive& pri)
{
    if (!isEdgeLines && pri.material >= 0 && model.materials[pri.material].baseColorTextureIndex >= 0)
    {
        kame::ogl::setShader(gShaderTexture);
        drawModelWithTexture(positions, model, pri);
        return;
    }

    if (isEdgeLines)
    {
        kame::ogl::setShader(gShaderDrawLines);
    }
    else
    {
        kame::ogl::setShader(gShaderFrontFace);
    }

    std::copy(pri.getIndices().begin(), pri.getIndices().end(), gIndices.begin());

    gVBO->setBuffer(positions);
    gIBO->setBuffer(gIndices);
    kame::ogl::VertexArrayObject vao;
    vao.begin()
        .bindAttribute(gShaderTexture->getAttribLocation("vPos"), gVBO, 3, 3 * sizeof(float), 0)
        .bindIndexBuffer(gIBO)
        .end();
    vao.drawElements(pri.mode, pri.getIndices().size(), GL_UNSIGNED_INT);
}

void loadTextures(const kame::gltf::Gltf* gltf)
{
    for (auto& t : gltf->textures)
    {
        assert(t.hasSource);
        kame::ogl::Texture2D* tex;
        auto& img = gltf->images[t.source];
        if (img.hasURI)
        {
            std::filesystem::path path(gltf->basePath);
            path /= img.uri;
            tex = kame::ogl::loadTexture2D(path.string().c_str());
        }
        else
        {
            assert(img.hasBufferView);
            auto& bv = gltf->bufferViews[img.bufferView];
            auto& b = gltf->buffers[bv.buffer];
            tex = kame::ogl::loadTexture2DFromMemory(b.data(), b.byteLength);
        }

        if (t.hasSampler)
        {
            auto& sampler = gltf->samplers[t.sampler];
            if (sampler.hasMagFilter)
            {
                assert(sampler.magFilter == GL_NEAREST || sampler.magFilter == GL_LINEAR);
                tex->setTexParameteri(GL_TEXTURE_MAG_FILTER, sampler.magFilter);
            }
            if (sampler.hasMinFilter)
            {
                assert(sampler.minFilter == GL_NEAREST || sampler.minFilter == GL_LINEAR || sampler.minFilter == GL_NEAREST_MIPMAP_NEAREST || sampler.minFilter == GL_LINEAR_MIPMAP_NEAREST || sampler.minFilter == GL_NEAREST_MIPMAP_LINEAR || sampler.minFilter == GL_LINEAR_MIPMAP_LINEAR);
                if (kame::ogl::Context::getInstance().capability.ext_framebuffer_object)
                {

                    tex->setTexParameteri(GL_TEXTURE_MIN_FILTER, sampler.minFilter);
                    if (sampler.minFilter == GL_NEAREST_MIPMAP_NEAREST || sampler.minFilter == GL_LINEAR_MIPMAP_NEAREST || sampler.minFilter == GL_NEAREST_MIPMAP_LINEAR || sampler.minFilter == GL_LINEAR_MIPMAP_LINEAR)
                    {
                        tex->generateMipmap();
                    }
                }
                else
                {
                    GLenum minFilter = sampler.minFilter;
                    if (minFilter == GL_NEAREST_MIPMAP_NEAREST || minFilter == GL_NEAREST_MIPMAP_LINEAR)
                    {
                        minFilter = GL_NEAREST;
                    }
                    else if (minFilter == GL_LINEAR_MIPMAP_NEAREST || minFilter == GL_LINEAR_MIPMAP_LINEAR)
                    {
                        minFilter = GL_LINEAR;
                    }
                    tex->setTexParameteri(GL_TEXTURE_MIN_FILTER, minFilter);
                }
            }
            assert(sampler.wrapS == GL_CLAMP_TO_EDGE || sampler.wrapS == GL_MIRRORED_REPEAT || sampler.wrapS == GL_REPEAT);
            tex->setTexParameteri(GL_TEXTURE_WRAP_S, sampler.wrapS);
            assert(sampler.wrapT == GL_CLAMP_TO_EDGE || sampler.wrapT == GL_MIRRORED_REPEAT || sampler.wrapT == GL_REPEAT);
            tex->setTexParameteri(GL_TEXTURE_WRAP_T, sampler.wrapT);
        }

        gTextures.emplace_back(tex);
    }
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
    loadTextures(gltf);
    importMaterial(model, gltf);
    std::unordered_map<std::string, kame::squirtle::AnimationClip> clips;
    kame::squirtle::AnimationClip* activeClip = nullptr;
    float playTime = 0.0f;
    clips = importAnimation(gltf);
    kame::gltf::deleteGLTF(gltf);

    size_t numPos = 0;
    size_t numUV = 0;
    size_t numIndex = 0;
    for (auto& n : model->nodes)
    {
        if (n.meshID < 0)
        {
            continue;
        }

        Mesh& srcMesh = model->meshes[n.meshID];
        for (Primitive& pri : srcMesh.primitives)
        {
            numPos = std::max(numPos, pri.getBytesOfPositions());
            if (!pri.uvSets.empty())
            {
                numUV = std::max(numUV, pri.getBytesOfUV(0));
            }
            numIndex = std::max(numIndex, pri.getBytesOfIndices());
            if (gPositions.size() < pri.getPositions().size())
            {
                gPositions.resize(pri.getPositions().size());
            }
            if (!pri.uvSets.empty() && gUV.size() < pri.getUvSets()[0].size())
            {
                gUV.resize(pri.getUvSets()[0].size());
            }
            if (gIndices.size() < pri.getIndices().size())
            {
                gIndices.resize(pri.getIndices().size());
            }
        }
    }

    gVBO = kame::ogl::createVertexBuffer(numPos, GL_STREAM_DRAW);
    gVBOTexCoord = kame::ogl::createVertexBuffer(numUV, GL_STREAM_DRAW);
    gIBO = kame::ogl::createIndexBuffer(numIndex, GL_STREAM_DRAW);

    gShaderFrontFace = kame::ogl::createShader(vertGLSL, fragGLSL);
    gShaderTexture = kame::ogl::createShader(vertTexGLSL, fragTexGLSL);
    gShaderDrawLines = kame::ogl::createShader(vertGLSL, drawLinesGLSL);

    // for turntable rotation
    kame::squirtle::CameraOrbit orbitCamera(90.0f, 1280.0f, 720.0f);

    std::vector<const char*> items;
    items.reserve(clips.size());
    for (auto& [_, clip] : clips)
    {
        items.emplace_back(clip.name.c_str());
    }
    int itemCurrent = 0;
    int itemSelect = 0;
    if (model->isSkinnedMesh() && !clips.empty())
    {
        activeClip = &clips[items[itemCurrent]];
    }

    double prevTime = win.getElapsedTime();

    for (;;)
    {
        win.updateInput();
        auto state = win.getState();
        if (state.isCloseRequest)
            break;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        // ImGui::ShowDemoWindow();

        ImGui::ListBox(fmt::format("{} clips", items.size()).data(), &itemSelect, items.data(), items.size(), 10);
        if (itemCurrent != itemSelect)
        {
            itemCurrent = itemSelect;
            activeClip = &clips[items[itemCurrent]];
            playTime = 0.0f;
        }
        float dt = win.getElapsedTime() - prevTime;
        prevTime = win.getElapsedTime();

        if (activeClip)
        {
            playTime += dt;
            playTime = kame::squirtle::animate(*activeClip, model->nodes, playTime);
        }

        bool isHover = ImGui::IsItemHovered() || ImGui::IsWindowHovered();
        if (!isHover)
        {
            orbitCamera.update(state);
        }

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

        kame::ogl::setShader(gShaderFrontFace);
        gShaderFrontFace->setMatrix("uView", orbitCamera.getViewMatrix());
        gShaderFrontFace->setMatrix("uProj", orbitCamera.getProjectionMatrix());
        gShaderFrontFace->setMatrix("uModel", orbitCamera.getModelMatrix());
        kame::ogl::setShader(gShaderTexture);
        gShaderTexture->setMatrix("uView", orbitCamera.getViewMatrix());
        gShaderTexture->setMatrix("uProj", orbitCamera.getProjectionMatrix());
        gShaderTexture->setMatrix("uModel", orbitCamera.getModelMatrix());
        kame::ogl::setShader(gShaderDrawLines);
        gShaderDrawLines->setMatrix("uView", orbitCamera.getViewMatrix());
        gShaderDrawLines->setMatrix("uProj", orbitCamera.getProjectionMatrix());
        gShaderDrawLines->setMatrix("uModel", orbitCamera.getModelMatrix());

        isEdgeLines = false;
        model->draw(gPositions, drawModel);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDepthMask(GL_FALSE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(0.0f, -40.0f);
        depthState = kame::ogl::DepthStencilStateBuilder()
                         .depthFunc(GL_LEQUAL)
                         .build();
        kame::ogl::setDepthStencilState(depthState);
        isEdgeLines = true;
        model->draw(gPositions, drawModel);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        win.swapWindow();
    }

    for (auto* tex : gTextures)
    {
        kame::ogl::deleteTexture2D(tex);
    }

    kame::ogl::deleteVertexBuffer(gVBO);
    kame::ogl::deleteVertexBuffer(gVBOTexCoord);
    kame::ogl::deleteIndexBuffer(gIBO);

    kame::ogl::deleteShader(gShaderDrawLines);
    kame::ogl::deleteShader(gShaderTexture);
    kame::ogl::deleteShader(gShaderFrontFace);
    win.closeWindow();
    kame::kameShutdown();
    return 0;
}

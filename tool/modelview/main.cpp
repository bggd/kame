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

#include "../common/common.hpp"

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

void updateGlobalXForm(Model& model, int id)
{
    Node& node = model.nodes[id];
    auto local = node.updateLocalXForm();
    auto global = kame::math::Matrix::identity();
    if (node.parent >= 0)
    {
        global = model.nodes[node.parent].globalXForm;
    }
    else
    {
        global = model.nodes.back().globalXForm;
    }
    node.globalXForm = local * global;
    for (auto c : node.children)
    {
        updateGlobalXForm(model, c);
    }
}

void drawModel(kame::ogl::Shader* shader, const Model& model, GLenum mode)
{
    kame::ogl::setShader(shader);
    GLint loc = shader->getAttribLocation("vPos");
    for (auto& n : model.nodes)
    {
        if (n.meshID < 0)
        {
            continue;
        }
        shader->setMatrix("uModel", n.globalXForm);
        auto& vbo = model.vboMeshes[n.meshID];
        auto vao = kame::ogl::VertexArrayObjectBuilder()
                       .bindAttribute(loc, vbo.vboPositions, 3, 3 * sizeof(float), 0)
                       .bindIndexBuffer(vbo.iboIndices)
                       .build();
        vao.drawElements(mode, vbo.numIndex, GL_UNSIGNED_INT);
    }
}

using namespace kame::math;
using namespace kame::math::helper;

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
    win.forceGLVersion(3, 3, false);
    win.setWindowFlags(SDL_WINDOW_RESIZABLE);
    win.openWindow("modelview", 1280, 720);
    win.setVsync(true);

    kame::gltf::Gltf* gltf = kame::gltf::loadGLTF(argv[1]);
    Model model;
    model.meshes.reserve(gltf->meshes.size());
    model.vboMeshes.reserve(gltf->meshes.size());
    for (auto& m : gltf->meshes)
    {
        model.meshes.emplace_back();
        auto& mesh = model.meshes.back();

        mesh.positions = toVertexPositions(gltf, m);
        mesh.uvSets = toVertexUVSets(gltf, m);
        mesh.indices = toVertexIndices(gltf, m);

        model.vboMeshes.emplace_back();
        auto& vbo = model.vboMeshes.back();
        vbo.initVBOMesh(mesh);
    }
    model.nodes.resize(gltf->nodes.size() + 1);
    int nodeID = 0;
    for (auto& n : gltf->nodes)
    {
        Node& node = model.nodes[nodeID];
        if (n.hasMesh)
        {
            node.meshID = n.mesh;
        }
        if (n.hasTranslation)
        {
            node.position.x = n.translation[0];
            node.position.y = n.translation[1];
            node.position.z = n.translation[2];
        }
        if (n.hasRotation)
        {
            node.rotation.x = n.rotation[0];
            node.rotation.y = n.rotation[1];
            node.rotation.z = n.rotation[2];
            node.rotation.w = n.rotation[3];
        }
        if (n.hasScale)
        {
            node.scale.x = n.scale[0];
            node.scale.y = n.scale[1];
            node.scale.z = n.scale[2];
        }
        node.children.reserve(n.children.size());
        for (auto c : n.children)
        {
            model.nodes[c].parent = nodeID;
            node.children.push_back(c);
        }
        ++nodeID;
    }
    nodeID = 0;
    for (auto& node : model.nodes)
    {
        if (nodeID >= gltf->nodes.size())
        {
            break;
        }
        if (node.parent < 0)
        {
            auto& root = model.nodes.back();
            root.children.push_back(nodeID);
        }
        ++nodeID;
    }
    kame::gltf::deleteGLTF(gltf);

    kame::ogl::Shader* shaderFrontFace = kame::ogl::createShader(vertGLSL, fragGLSL);
    kame::ogl::Shader* shaderDrawLines = kame::ogl::createShader(vertGLSL, drawLinesGLSL);

    // for turntable rotation
    Matrix modelMtx = Matrix::identity();
    int prevMouseX = 0, prevMouseY = 0;
    float sensitivity = 0.4f; // degree
    float rotX = 0.0f, rotY = 0.0f;
    float zoom = 0.0f;

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
        }
        else if (state.isDownRMB)
        {
            zoom += 0.05f * float(relMouseX);
            if (zoom < 0.0f)
            {
                zoom = 0.0f;
            }
        }

        prevMouseX = state.mouseX;
        prevMouseY = state.mouseY;

        model.nodes.back().globalXForm = modelMtx;
        updateGlobalXForm(model, model.nodes.size() - 1);

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

        drawModel(shaderFrontFace, model, GL_TRIANGLES);

        kame::ogl::setShader(shaderDrawLines);
        shaderDrawLines->setMatrix("uView", view);
        shaderDrawLines->setMatrix("uProj", proj);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDepthMask(GL_FALSE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(0.0f, -40.0f);
        depthState = kame::ogl::DepthStencilStateBuilder()
                         .depthFunc(GL_LEQUAL)
                         .build();
        kame::ogl::setDepthStencilState(depthState);
        drawModel(shaderDrawLines, model, GL_TRIANGLES);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        win.swapWindow();
    }

    kame::ogl::deleteShader(shaderDrawLines);
    kame::ogl::deleteShader(shaderFrontFace);
    win.closeWindow();
    kame::kameShutdown();
    return 0;
}
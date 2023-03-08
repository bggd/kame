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

void updateAnimation(Model& model, AnimationClip& clip, float time)
{
    for (auto& c : clip.channels)
    {
        if (c.targetID < 0)
        {
            continue;
        }

        Node& node = model.nodes[c.targetID];

        auto& s = clip.samplers[c.samplerID];
        if (s.inputs.size() > s.outputsVec4.size())
        {
            continue;
        }

        for (size_t i = 0; i < s.outputsVec4.size() - 1; ++i)
        {
            if ((time >= s.inputs[i]) && (time <= s.inputs[i + 1]))
            {
                float u = std::max(0.0f, time - s.inputs[i]) / (s.inputs[i + 1] - s.inputs[i]);
                if (u <= 1.0f)
                {
                    // TODO: STEP and CUBICSPLINE interp
                    switch (c.path)
                    {
                        case AnimationClip::Channel::PathType::TRANSLATION: {
                            auto trans = kame::math::Vector4::lerp(s.outputsVec4[i], s.outputsVec4[i + 1], u);
                            node.position = kame::math::Vector3(trans.x, trans.y, trans.z);
                            break;
                        }
                        case AnimationClip::Channel::PathType::SCALE: {
                            auto scale = kame::math::Vector4::lerp(s.outputsVec4[i], s.outputsVec4[i + 1], u);
                            node.scale = kame::math::Vector3(scale.x, scale.y, scale.z);
                            break;
                        }
                        case AnimationClip::Channel::PathType::ROTATION: {
                            auto rot = kame::math::Quaternion::slerp(s.outputsVec4[i], s.outputsVec4[i + 1], u);
                            node.rotation = kame::math::Quaternion::normalize(rot);
                            break;
                        }
                    }
                    model.animationUpdated = true;
                }
            }
        }
    }
}

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

void updateSkinMatrices(Model& model)
{
    for (auto& skin : model.skins)
    {
        for (uint32_t i = 0; i < skin.joints.size(); ++i)
        {
            Node& joint = model.nodes[skin.joints[i]];
            skin.matrices[i] = skin.inverseBindMatrices[i] * joint.globalXForm;
        }
    }
}

void drawModel(kame::ogl::Shader* shader, Model& model, GLenum mode)
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

        auto invertMtx = kame::math::Matrix::invert(n.globalXForm);
        if (n.skinID >= 0)
        {
            Skin& s = model.skins[n.skinID];
            static std::vector<kame::math::Matrix> skinMatrices;
            skinMatrices.resize(s.matrices.size());
            for (int i = 0; i < s.matrices.size(); ++i)
            {
                skinMatrices[i] = s.matrices[i] * invertMtx;
            }

            Mesh& srcMesh = model.meshes[n.meshID];
            static Mesh dstMesh;
            dstMesh.positions.clear();
            dstMesh.positions.resize(srcMesh.positions.size());
            for (auto i : srcMesh.indices)
            {
                auto vPos = srcMesh.positions[i];
                auto vJoint = srcMesh.joints[i];
                auto vWeight = srcMesh.weights[i];

                // clang-format off
                auto skinMtx = 
                      skinMatrices[vJoint[0]] * vWeight.x
                    + skinMatrices[vJoint[1]] * vWeight.y
                    + skinMatrices[vJoint[2]] * vWeight.z
                    + skinMatrices[vJoint[3]] * vWeight.w;
                // clang-format on

                dstMesh.positions[i] = kame::math::Vector3::transform(vPos, skinMtx);
            }
            vbo.vboPositions->setBuffer((const float*)&dstMesh.positions[0]);
        }

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
        mesh.joints = toVertexJoints(gltf, m);
        mesh.weights = toVertexWeights(gltf, m);
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
        if (n.hasSkin)
        {
            node.skinID = n.skin;
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
    model.clips.reserve(gltf->animations.size());
    for (auto& a : gltf->animations)
    {
        model.clips.emplace_back();
        AnimationClip& clip = model.clips.back();
        clip.channels.reserve(a.channels.size());
        for (auto& c : a.channels)
        {
            clip.channels.emplace_back();
            AnimationClip::Channel& chan = clip.channels.back();
            chan.samplerID = c.sampler;
            if (c.target.hasNode)
            {
                chan.targetID = c.target.node;
            }
            if (c.target.path == "translation")
            {
                chan.path = AnimationClip::Channel::PathType::TRANSLATION;
            }
            else if (c.target.path == "rotation")
            {
                chan.path = AnimationClip::Channel::PathType::ROTATION;
            }
            else if (c.target.path == "scale")
            {
                chan.path = AnimationClip::Channel::PathType::SCALE;
            }
        }
        clip.samplers.reserve(a.samplers.size());
        for (auto& s : a.samplers)
        {
            clip.samplers.emplace_back();
            AnimationClip::Sampler& smp = clip.samplers.back();
            if (s.interpolation == "LINEAR")
            {
                smp.interpolation = AnimationClip::Sampler::InterpolationType::LINEAR;
            }
            else if (s.interpolation == "STEP")
            {
                smp.interpolation = AnimationClip::Sampler::InterpolationType::STEP;
            }
            else if (s.interpolation == "CUBICSPLINE")
            {
                smp.interpolation = AnimationClip::Sampler::InterpolationType::CUBICSPLINE;
            }

            {
                auto& acc = gltf->accessors[s.input];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                smp.inputs.reserve(acc.count);
                assert(acc.componentType == GL_FLOAT);
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    auto v = ((float*)(b.binaryData.data() + bv.byteOffset + acc.byteOffset))[i];
                    smp.inputs.push_back(v);
                    clip.startTime = std::min(clip.startTime, v);
                    clip.endTime = std::max(clip.endTime, v);
                }
            }
            {
                auto& acc = gltf->accessors[s.output];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                smp.outputsVec4.reserve(acc.count);
                assert(acc.componentType == GL_FLOAT);
                if (acc.type == "VEC3")
                {
                    for (unsigned int i = 0; i < acc.count; ++i)
                    {
                        auto v = ((kame::math::Vector3*)(b.binaryData.data() + bv.byteOffset + acc.byteOffset))[i];
                        smp.outputsVec4.push_back(kame::math::Vector4(v, 0.0f));
                    }
                }
                else if (acc.type == "VEC4")
                {
                    for (unsigned int i = 0; i < acc.count; ++i)
                    {
                        auto v = ((kame::math::Vector4*)(b.binaryData.data() + bv.byteOffset + acc.byteOffset))[i];
                        smp.outputsVec4.push_back(v);
                    }
                }
            }
        }
    }
    model.skins.reserve(gltf->skins.size());
    for (auto& s : gltf->skins)
    {
        model.skins.emplace_back();
        auto& skin = model.skins.back();
        assert(s.hasInverseBindMatrices);
        auto& acc = gltf->accessors[s.inverseBindMatrices];
        auto& bv = gltf->bufferViews[acc.bufferView];
        auto& b = gltf->buffers[bv.buffer];
        skin.inverseBindMatrices.reserve(acc.count);
        for (unsigned int i = 0; i < acc.count; ++i)
        {
            auto v = ((kame::math::Matrix*)(b.binaryData.data() + bv.byteOffset + acc.byteOffset))[i];
            skin.inverseBindMatrices.push_back(v);
        }
        skin.joints.reserve(s.joints.size());
        for (auto& jID : s.joints)
        {
            skin.joints.push_back(jID);
        }
        skin.matrices.resize(s.joints.size());
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

    float playTime = 0.0f;

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

        playTime += 1.0 / 60.0f;
        if (playTime > model.clips[0].endTime)
        {
            playTime = model.clips[0].startTime + (model.clips[0].endTime - playTime);
        }
        updateAnimation(model, model.clips[0], playTime);
        model.nodes.back()
            .globalXForm = modelMtx;
        updateGlobalXForm(model, model.nodes.size() - 1);
        updateSkinMatrices(model);

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
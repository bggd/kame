#pragma once

#include <kame/kame.hpp>
#include <array>
#include <cstdint>

namespace kame::squirtle {

using u16Array4 = std::array<uint16_t, 4>;
using u8Array4 = std::array<uint8_t, 4>;

struct Mesh {
    std::vector<kame::math::Vector3> positions;
    std::vector<std::vector<kame::math::Vector2>> uvSets;
    std::vector<u16Array4> joints;
    std::vector<kame::math::Vector4> weights;
    std::vector<unsigned int> indices;
};

struct VBODoubleBuffer {
    int numBuffer = 2;
    int currentBuffer = 0;
    std::vector<kame::ogl::VertexBuffer*> buffers;

    VBODoubleBuffer(int num_buffer = 2);

    void initVBODoubleBuffer(GLsizeiptr numBytes, GLenum usage);
    void setBuffer(const float* vertices);
    kame::ogl::VertexBuffer* getCurrentVBO();
    void shutDownVBODoubleBuffer();
};

struct VBOMesh {
    VBODoubleBuffer vboPositions;
    std::vector<kame::ogl::VertexBuffer*> vboUVSets;
    kame::ogl::IndexBuffer* iboIndices = nullptr;
    size_t numIndex = 0;

    void initVBOMesh(const Mesh& mesh);

    void shutdownVBOMesh();
};

void cleanupVBOMeshes(std::vector<VBOMesh>& vboMeshes);

std::vector<kame::math::Vector3> toVertexPositions(const kame::gltf::Gltf* gltf, const kame::gltf::Mesh& m);
std::vector<std::vector<kame::math::Vector2>> toVertexUVSets(const kame::gltf::Gltf* gltf, const kame::gltf::Mesh& m);
std::vector<u16Array4> toVertexJoints(const kame::gltf::Gltf* gltf, const kame::gltf::Mesh& m);
std::vector<kame::math::Vector4> toVertexWeights(const kame::gltf::Gltf* gltf, const kame::gltf::Mesh& m);
std::vector<unsigned int> toVertexIndices(const kame::gltf::Gltf* gltf, const kame::gltf::Mesh& m);

struct Node {
    kame::math::Vector3 position = kame::math::Vector3::zero();
    kame::math::Vector3 scale = kame::math::Vector3::one();
    kame::math::Quaternion rotation = kame::math::Quaternion::identity();

    kame::math::Matrix localXForm = kame::math::Matrix::identity();
    kame::math::Matrix globalXForm = kame::math::Matrix::identity();

    int meshID = -1;
    int skinID = -1;

    int parent = -1;
    std::vector<int> children;

    kame::math::Matrix updateLocalXForm()
    {
        localXForm = kame::math::Matrix::createScale(scale) * kame::math::Matrix::CreateFromQuaternion(rotation) * kame::math::Matrix::createTranslation(position);
        return localXForm;
    }
};

struct AnimationClip {
    struct Channel {
        int targetID = -1;
        enum PathType { TRANSLATION,
                        ROTATION,
                        SCALE };
        PathType path;
        uint32_t samplerID;
    };
    struct Sampler {
        enum InterpolationType { LINEAR,
                                 STEP,
                                 CUBICSPLINE };
        InterpolationType interpolation;
        std::vector<float> inputs;
        std::vector<kame::math::Vector4> outputsVec4;
    };

    std::string name;
    std::vector<Channel> channels;
    std::vector<Sampler> samplers;
    float startTime = std::numeric_limits<float>::max();
    float endTime = std::numeric_limits<float>::min();
};

struct Skin {
    std::vector<kame::math::Matrix> inverseBindMatrices;
    std::vector<int> joints;
    std::vector<kame::math::Matrix> matrices;
};

struct Model {
    std::vector<Mesh> meshes;
    std::vector<VBOMesh> vboMeshes;
    std::vector<Node> nodes;
    std::vector<Skin> skins;
    std::vector<AnimationClip> clips;
    bool animationUpdated = false;

    void setGlobalXForm(kame::math::Matrix xform)
    {
        nodes.back().globalXForm = xform;
    }
};

Model importModel(const kame::gltf::Gltf* gltf);
void updateAnimation(Model& model, std::vector<Node>& nodes, AnimationClip& clip, float time);
void updateGlobalXForm(Model& model);
void updateSkinMatrices(Model& model);
void updateVBOMeshes(Model& model);

} // namespace kame::squirtle

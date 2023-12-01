#pragma once

#include <kame/kame.hpp>
#include <array>
#include <cstdint>
#include <unordered_map>
#include <functional>

#include "camera.hpp"
#include "material.hpp"

namespace kame::squirtle {

using u16Array4 = std::array<uint16_t, 4>;
using u8Array4 = std::array<uint8_t, 4>;

struct Primitive {
    std::vector<kame::math::Vector3> positions;
    std::vector<std::vector<kame::math::Vector2>> uvSets;
    std::vector<u16Array4> joints;
    std::vector<kame::math::Vector4> weights;
    std::vector<unsigned int> indices;
    int material = -1;
    GLenum mode = GL_TRIANGLES;
    int id = -1;

    size_t getBytesOfPositions() const
    {
        return sizeof(float) * 3 * positions.size();
    }

    size_t getBytesOfUV(size_t i)
    {
        return sizeof(float) * 2 * uvSets[i].size();
    }

    size_t getBytesOfIndices() const
    {
        return sizeof(unsigned int) * indices.size();
    }
};

struct Mesh {
    std::vector<Primitive> primitives;
};

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
        localXForm = kame::math::Matrix::createScale(scale) * kame::math::Matrix::createFromQuaternion(rotation) * kame::math::Matrix::createTranslation(position);
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

using DrawCB = std::function<void(const std::vector<kame::math::Vector3>& positions, const Model&, const Primitive&)>;

struct Model {
    std::vector<Mesh> meshes;
    std::vector<Node> nodes;
    std::vector<Skin> skins;
    std::vector<Texture> textures;
    std::vector<Material> materials;
    std::unordered_map<std::string, AnimationClip> clips;
    AnimationClip* activeClip = nullptr;
    float playTime = 0.0f;
    bool isPlay = false;
    bool _isSkinnedMesh = false;

    bool hasAnimation()
    {
        return clips.empty() == false;
    }

    bool isSkinnedMesh()
    {
        return _isSkinnedMesh;
    }

    void setAnimationClip(std::string name);
    void setAnimationClip(const char* name) { setAnimationClip(std::string(name)); }
    void playAnimation();
    void updateAnimation(float dt);

    void draw(std::vector<kame::math::Vector3>& positions, DrawCB fn);
};

Model* importModel(const kame::gltf::Gltf* gltf);

} // namespace kame::squirtle

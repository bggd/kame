#pragma once

#include <kame/kame.hpp>
#include <array>
#include <cstdint>
#include <unordered_map>
#include <functional>
#include <variant>

#include "camera.hpp"
#include "material.hpp"
#include "animation.hpp"
#include "instance.hpp"

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

    const std::vector<kame::math::Vector3>& getPositions() const;
    const std::vector<std::vector<kame::math::Vector2>>& getUvSets() const;
    const std::vector<u16Array4>& getJoints() const;
    const std::vector<kame::math::Vector4>& getWeights() const;
    const std::vector<unsigned int>& getIndices() const;

    size_t getBytesOfPositions() const;
    size_t getBytesOfUV(size_t i);
    size_t getBytesOfIndices() const;
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

struct Skin {
    std::vector<kame::math::Matrix> inverseBindMatrices;
    std::vector<int> joints;
    std::vector<kame::math::Matrix> matrices;
};

struct UpdateData {
    const std::vector<kame::math::Vector3>& positions;
    const Model& model;
    const Primitive& primitive;
};

using UpdateCB = std::function<void(const UpdateData&)>;

struct Model {
    std::vector<Mesh> meshes;
    std::vector<Node> nodes;
    std::vector<Skin> skins;
    std::vector<Material> materials;
    std::vector<Texture> textures;
    std::vector<Image> images;
    bool _isSkinnedMesh = false;

    bool isSkinnedMesh()
    {
        return _isSkinnedMesh;
    }

    void update(std::vector<kame::math::Vector3>& positions, UpdateCB fn);
};

Model* importModel(const kame::gltf::Gltf* gltf);
float animate(AnimationClip& clip, std::vector<Node>& nodes, float playTime);

} // namespace kame::squirtle

#pragma once
#include <kame/kame.hpp>
#include <array>
#include <vector>
#include <cstdint>

#include <pystring.h>

using u16Array4 = std::array<uint16_t, 4>;
using u8Array4 = std::array<uint8_t, 4>;

struct Mesh {
    std::vector<kame::math::Vector3> positions;
    std::vector<std::vector<kame::math::Vector2>> uvSets;
    std::vector<u16Array4> joints;
    std::vector<kame::math::Vector4> weights;
    std::vector<unsigned int> indices;
};

struct VBOMesh {
    kame::ogl::VertexBuffer* vboPositions = nullptr;
    std::vector<kame::ogl::VertexBuffer*> vboUVSets;
    kame::ogl::IndexBuffer* iboIndices = nullptr;
    size_t numIndex = 0;

    void initVBOMesh(const Mesh& mesh)
    {
        vboPositions = kame::ogl::createVertexBuffer(mesh.positions.size() * 3 * sizeof(float), GL_STATIC_DRAW);
        vboPositions->setBuffer((const float*)&mesh.positions[0]);

        vboUVSets.reserve(mesh.uvSets.size());
        for (const auto& uv : mesh.uvSets)
        {
            auto* vboTexcoords = kame::ogl::createVertexBuffer(uv.size() * 2 * sizeof(float), GL_STATIC_DRAW);
            vboTexcoords->setBuffer((const float*)&uv[0]);
            vboUVSets.push_back(vboTexcoords);
        }

        iboIndices = kame::ogl::createIndexBuffer(mesh.indices.size() * sizeof(unsigned int), GL_STATIC_DRAW);
        iboIndices->setBuffer((const unsigned int*)&mesh.indices[0]);
        numIndex = mesh.indices.size();
    }

    void shutdownVBOMesh()
    {
        kame::ogl::deleteIndexBuffer(iboIndices);
        for (auto& uv : vboUVSets)
        {
            kame::ogl::deleteVertexBuffer(uv);
        }
        kame::ogl::deleteVertexBuffer(vboPositions);
    }
};

void cleanupVBOMeshes(std::vector<VBOMesh>& vboMeshes)
{
    for (auto& vbo : vboMeshes)
    {
        vbo.shutdownVBOMesh();
    }
}

std::vector<kame::math::Vector3> toVertexPositions(const kame::gltf::Gltf* gltf, const kame::gltf::Mesh& m)
{
    std::vector<kame::math::Vector3> positions;

    for (auto& pri : m.primitives)
    {
        for (auto& item : pri.attributes)
        {
            if (item.first == "POSITION")
            {
                auto& acc = gltf->accessors[item.second];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                positions.reserve(acc.count);
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    auto v = ((kame::math::Vector3*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    positions.push_back(v);
                }
            }
        }
    }

    return positions;
}

std::vector<std::vector<kame::math::Vector2>> toVertexUVSets(const kame::gltf::Gltf* gltf, const kame::gltf::Mesh& m)
{
    std::vector<std::vector<kame::math::Vector2>> uvSets;

    for (auto& pri : m.primitives)
    {
        for (auto& item : pri.attributes)
        {
            if (pystring::startswith(item.first, "TEXCOORD"))
            {
                uvSets.emplace_back();
                int uvid = stoi(pystring::lstrip(item.first, "TEXCOORD_"));
                assert(uvid >= 0);
                assert(uvid < uvSets.size());

                auto& acc = gltf->accessors[item.second];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                uvSets[uvid].reserve(acc.count);
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    if (acc.componentType == GL_FLOAT)
                    {
                        auto v = ((kame::math::Vector2*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        uvSets[uvid].push_back(v);
                    }
                    else if (acc.componentType == GL_UNSIGNED_BYTE)
                    {
                        auto e = ((unsigned char*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        uvSets[uvid].push_back(e / 255.0f);
                    }
                    else if (acc.componentType == GL_UNSIGNED_SHORT)
                    {
                        auto e = ((unsigned short*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        uvSets[uvid].push_back(e / 65535.0f);
                    }
                }
            }
        }
    }

    return uvSets;
}

std::vector<u16Array4> toVertexJoints(const kame::gltf::Gltf* gltf, const kame::gltf::Mesh& m)
{
    std::vector<std::array<uint16_t, 4>> joints;

    for (auto& pri : m.primitives)
    {
        for (auto& item : pri.attributes)
        {
            if (item.first == "JOINTS_0")
            {
                auto& acc = gltf->accessors[item.second];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                joints.reserve(acc.count);
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    if (acc.componentType == GL_UNSIGNED_BYTE)
                    {
                        auto e = ((u8Array4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        u16Array4 v;
                        v[0] = e[0];
                        v[1] = e[1];
                        v[2] = e[2];
                        v[3] = e[3];
                        joints.push_back(v);
                    }
                    else if (acc.componentType == GL_UNSIGNED_SHORT)
                    {
                        auto v = ((u16Array4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        joints.push_back(v);
                    }
                }
            }
        }
    }

    return joints;
}

std::vector<kame::math::Vector4> toVertexWeights(const kame::gltf::Gltf* gltf, const kame::gltf::Mesh& m)
{
    std::vector<kame::math::Vector4> weights;

    for (auto& pri : m.primitives)
    {
        for (auto& item : pri.attributes)
        {
            if (item.first == "WEIGHTS_0")
            {
                auto& acc = gltf->accessors[item.second];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                weights.reserve(acc.count);
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    if (acc.componentType == GL_FLOAT)
                    {
                        auto v = ((kame::math::Vector4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        weights.push_back(v);
                    }
                    else if (acc.componentType == GL_UNSIGNED_BYTE)
                    {
                        auto e = ((u8Array4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        kame::math::Vector4 v;
                        v.x = e[0] / 255.0f;
                        v.y = e[1] / 255.0f;
                        v.z = e[2] / 255.0f;
                        v.w = e[3] / 255.0f;
                        weights.push_back(v);
                    }
                    else if (acc.componentType == GL_UNSIGNED_SHORT)
                    {
                        auto e = ((u16Array4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        kame::math::Vector4 v;
                        v.x = e[0] / 65535.0f;
                        v.y = e[1] / 65535.0f;
                        v.z = e[2] / 65535.0f;
                        v.w = e[3] / 65535.0f;
                        weights.push_back(v);
                    }
                }
            }
        }
    }

    return weights;
}

std::vector<unsigned int> toVertexIndices(const kame::gltf::Gltf* gltf, const kame::gltf::Mesh& m)
{
    std::vector<unsigned int> indices;

    for (auto& pri : m.primitives)
    {
        if (pri.hasIndices)
        {
            auto& acc = gltf->accessors[pri.indices];
            auto& bv = gltf->bufferViews[acc.bufferView];
            auto& b = gltf->buffers[bv.buffer];
            indices.reserve(acc.count);
            for (unsigned int i = 0; i < acc.count; ++i)
            {
                if (acc.componentType == GL_UNSIGNED_BYTE)
                {
                    auto e = ((unsigned char*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    indices.push_back(e);
                }
                else if (acc.componentType == GL_UNSIGNED_SHORT)
                {
                    auto e = ((unsigned short*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    indices.push_back(e);
                }
                else if (acc.componentType == GL_UNSIGNED_INT)
                {
                    auto e = ((unsigned int*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    indices.push_back(e);
                }
            }
        }
    }

    return indices;
}

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

    std::vector<Channel> channels;
    std::vector<Sampler> samplers;
    float startTime = std::numeric_limits<float>::max();
    float endTime = std::numeric_limits<float>::lowest();
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
};

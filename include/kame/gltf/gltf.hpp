#pragma once

#include <cstdint>

#include <vector>
#include <string>
#include <utility>

#include "gltf_ext.hpp"
#include "gltf_material.hpp"

namespace kame::gltf {

using integer = uint64_t;

struct Buffer {
    std::string name;
    std::string uri;
    integer byteLength;
    std::vector<uint8_t> binaryData;
    const unsigned char* data() const
    {
        return binaryData.data();
    }
};

struct BufferView {
    std::string name;
    integer buffer;
    integer byteOffset = 0;
    integer byteLength;
    integer byteStride;
    integer target;
    bool hasByteOffset = false;
    bool hasByteStride = false;
    bool hasTarget = false;
};

struct Accessor {
    std::string name;
    integer bufferView = 0;
    integer byteOffset = 0;
    integer componentType;
    bool normalized = false;
    integer count;
    std::string type;
    std::vector<integer> max, min;
    bool hasBufferView = false;
    bool hasByteOffset = false;
    bool hasNormalized = false;
};

struct Mesh {

    struct Primitive {
        std::vector<std::pair<std::string, integer>> attributes;
        integer indices;
        integer material;
        integer mode = 4;
        bool hasIndices = false;
        bool hasMaterial = false;
        bool hasMode = false;
    };

    std::string name;
    std::vector<Primitive> primitives;
};

struct Node {
    std::string name;
    std::vector<integer> children;
    integer skin;
    integer mesh;
    float matrix[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
    float translation[3] = {0.0f, 0.0f, 0.0f};
    float rotation[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float scale[3] = {1.0f, 1.0f, 1.0f};
    Ext* extensions = nullptr;
    bool hasSkin = false;
    bool hasMesh = false;
    bool hasMatrix = false;
    bool hasTranslation = false;
    bool hasRotation = false;
    bool hasScale = false;
    bool hasExtensions = false;
};

struct Scene {
    std::string name;
    std::vector<integer> nodes;
};

struct Animation {

    struct Channel {

        struct Target {
            integer node;
            bool hasNode = false;
            std::string path;
        };

        integer sampler;
        Animation::Channel::Target target;
    };

    struct Sampler {
        integer input;
        std::string interpolation = "LINEAR";
        integer output;
    };

    std::string name;
    std::vector<Animation::Channel> channels;
    std::vector<Animation::Sampler> samplers;
};

struct Skin {
    std::string name;
    integer inverseBindMatrices;
    integer skeleton;
    std::vector<integer> joints;
    bool hasInverseBindMatrices = false;
    bool hasSkeleton = false;
};

struct Gltf {
    integer scene = 0;
    bool hasScene = false;
    std::vector<Scene> scenes;
    std::vector<Node> nodes;
    std::vector<Buffer> buffers;
    std::vector<BufferView> bufferViews;
    std::vector<Accessor> accessors;
    std::vector<Mesh> meshes;
    std::vector<Animation> animations;
    std::vector<Skin> skins;
    std::vector<Texture> textures;
    std::vector<Image> images;
    std::vector<Sampler> samplers;
    std::vector<Material> materials;
    std::string path;
    std::string basePath;
};

Gltf* loadGLTF(const char* path);
Gltf* loadGLTFFromMemory(const unsigned char* src, unsigned int len, const char* path = "");
void deleteGLTF(Gltf* gltf);

std::vector<uint8_t> decodeBase64(const std::string& str, integer start);

} // namespace kame::gltf

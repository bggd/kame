#pragma once

#include <cstdint>

#include <vector>
#include <string>
#include <utility>

namespace kame::gltf {

using integer = uint64_t;

struct Texture {
    integer sampler;
    integer source;
    bool hasSampler = false;
    bool hasSource = false;
};

struct Image {
    std::string uri;
    std::string mimeType;
    integer bufferView;
    bool hasURI = false;
    bool hasMimeType = false;
    bool hasBufferView = false;
};

struct Sampler {
    integer magFilter;
    integer minFilter;
    integer wrapS = 10497;
    integer wrapT = 10497;
    bool hasMagFilter = false;
    bool hasMinFilter = false;
};

struct Material {
    struct TextureInfo {
        integer index;
        integer texCoord = 0;
    };

    struct PBRMetallicRoughness {
        float baseColorFactor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        TextureInfo baseColorTexture;
        float metallicFactor = 1.0f;
        float roughnessFactor = 1.0f;
        TextureInfo metallicRoughnessTexture;
        bool hasBaseColorTexture = false;
        bool hasMetallicRoughnessTexture = false;
    };

    struct NormalTextureInfo : TextureInfo {
        float scale = 1.0f;
    };

    PBRMetallicRoughness pbrMetallicRoughness;
    NormalTextureInfo normalTexture;
    std::string alphaMode = "OPAQUE";
    float alphaCutoff = 0.5f;
    bool doubleSided = false;
    bool hasNormalTexture = false;
};

} // namespace kame::gltf

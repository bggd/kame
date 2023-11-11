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
        float baseColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        TextureInfo textureInfo;
        float metallicFactor = 1.0f;
        float roughnessFactor = 1.0f;
        TextureInfo metallicRoughnessTexture;
        bool hasMetallicRoughnessTexture = false;
    };

    PBRMetallicRoughness pbrMetallicRoughness;
    bool doubleSided = false;
};

} // namespace kame::gltf

#pragma once

#include <kame/kame.hpp>

namespace kame::squirtle {

enum AlphaMode {
    kALPHA_MODE_OPAQUE,
    kALPHA_MODE_MASK,
    kALPHA_MODE_BLEND
};

struct Image {
    std::string mimeType;
    std::string url;
};

struct Texture {
    int imageIndex = -1;
    GLenum magFilter = GL_LINEAR;
    GLenum minFilter = GL_NEAREST_MIPMAP_LINEAR;
    GLenum wrapS = GL_REPEAT;
    GLenum wrapT = GL_REPEAT;
};

struct Material {
    kame::math::Vector4 baseColorFactor = kame::math::Vector4::one();
    int baseColorTextureIndex = -1;
    int baseColorTexCoord = -1;
    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    int normalTextureIndex = -1;
    int normalTextureTexCoord = -1;
    float normalTextureScale = 1.0f;
    AlphaMode alphaMode = kALPHA_MODE_OPAQUE;
    float alphaCutoff = 0.5f;
    bool doubleSided;
};

struct Model;

void importMaterial(Model* model, const kame::gltf::Gltf* gltf);

} // namespace kame::squirtle

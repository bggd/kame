#pragma once

#include <kame/kame.hpp>

namespace kame::squirtle {

struct Texture {
    kame::ogl::Texture2D* gpuTex = nullptr;
};

enum AlphaMode {
    kALPHA_MODE_OPAQUE,
    kALPHA_MODE_MASK,
    kALPHA_MODE_BLEND
};

struct Material {
    kame::math::Vector4 baseColorFactor = kame::math::Vector4::one();
    int baseColorTexture = -1;
    int baseColorTexCoord = -1;
    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    AlphaMode alphaMode = kALPHA_MODE_OPAQUE;
    float alphaCutoff = 0.5f;
    bool doubleSided;
};

struct Model;

void importMaterial(Model* model, const kame::gltf::Gltf* gltf);

} // namespace kame::squirtle

#pragma once

#include <kame/kame.hpp>

namespace kame::squirtle {

struct Texture {
    kame::ogl::Texture2D* gpuTex = nullptr;
};

struct Material {
    kame::math::Vector4 baseColorFactor = kame::math::Vector4::one();
    int baseColorTexture = -1;
    int baseColorTexCoord = -1;
    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    bool doubleSided;
};

struct Model;

void importMaterial(Model* model, const kame::gltf::Gltf* gltf);

} // namespace kame::squirtle

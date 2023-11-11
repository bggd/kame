#pragma once

#include <kame/kame.hpp>

namespace kame::squirtle {

struct Material {
    kame::math::Vector4 baseColorFactor = kame::math::Vector4::one();
    int baseColorTexture = -1;
    int baseColorSampler = -1;
    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
};

} // namespace kame::squirtle

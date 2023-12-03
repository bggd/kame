#pragma once

#include <kame/math/math.hpp>
#include <kame/gltf/gltf.hpp>

#include <vector>

namespace kame::squirtle {

struct InstanceAttributes {
    std::vector<kame::math::Vector3> tranlations;
    std::vector<kame::math::Vector4> rotations;
    std::vector<kame::math::Vector3> scales;
};

InstanceAttributes importInstanceAttributes(const kame::gltf::Gltf* gltf);
std::vector<kame::math::Matrix> toInstanceMatrices(const InstanceAttributes& attributes);

} // namespace kame::squirtle

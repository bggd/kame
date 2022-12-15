#pragma once

#include <kame/gltf/gltf.hpp>

#include "gltf_node.hpp"

namespace kame::squirtle::helper {

kame::squirtle::GltfNode* loadGltfNode(const kame::gltf::Gltf* gltf);

} // namespace kame::squirtle::helper

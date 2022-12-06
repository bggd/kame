#pragma once

#include <kame/gltf/gltf.hpp>

#include "node/node.hpp"
#include "node/camera_node.hpp"
#include "node/mesh_node.hpp"
#include "engine.hpp"

namespace kame::squirtle {

void loadMeshNodeFromGLTF(kame::squirtle::Node* parent, const kame::gltf::Gltf* gltf);

} // namespace kame::squirtle

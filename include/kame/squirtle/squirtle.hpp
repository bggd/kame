#pragma once

#include <kame/gltf/gltf.hpp>

namespace kame::squirtle {
}

#include "node/node.hpp"
#include "node/camera_node.hpp"
#include "node/mesh_node.hpp"
#include "engine.hpp"

namespace kame::squirtle {

void loadMeshNodeFromGLTF(kame::squirtle::Node* parent, const kame::gltf::Gltf* gltf);

struct SceneTree {
    kame::squirtle::Node* root = nullptr;
};

} // namespace kame::squirtle

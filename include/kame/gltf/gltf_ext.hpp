#pragma once

#include "gltf_ext_mesh_gpu_instancing.hpp"

namespace kame::gltf {

struct Ext {
    ExtMeshGPUInstancing* EXT_mesh_gpu_instancing = nullptr;
    bool hasEXT_mesh_gpu_instancing = false;
};

} // namespace kame::gltf

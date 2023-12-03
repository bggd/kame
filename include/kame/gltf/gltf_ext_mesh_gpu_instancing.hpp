#pragma once

#include <cstdint>

#include <vector>
#include <string>

namespace kame::gltf {

using integer = uint64_t;

struct ExtMeshGPUInstancing {
    std::vector<std::pair<std::string, integer>> attributes;
};

} // namespace kame::gltf

#pragma once

#include <kame/math/math.hpp>
#include <vector>

namespace kame::assimp {

struct Mesh {
    std::vector<kame::math::Vector3f> positions;
    std::vector<kame::math::Vector2f> texCoords;
    std::vector<unsigned int> indices;
};

struct Model {
    Mesh mesh;
};

Model* loadModel(const char* path);
Model* loadModelFromMemory(const void* src, size_t len, const char* hint);
void deleteModel(Model* model);

} // namespace kame::assimp
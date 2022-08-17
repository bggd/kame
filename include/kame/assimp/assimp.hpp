#pragma once

#include <kame/math/math.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <array>

#include <assimp/scene.h>

namespace kame::assimp {

struct BoneInfluence {
    unsigned int boneID;
    float weight;
};

struct Mesh {
    std::vector<kame::math::Vector3f> positions;
    std::vector<std::vector<kame::math::Vector2f>> texCoords;
    std::vector<std::vector<BoneInfluence>> influences;
    std::vector<unsigned int> indices;
};

struct Bone {
    std::string name;
    aiMatrix4x4 offsetMatrix = aiMatrix4x4();
    int id;
    Bone* parent = nullptr;
    std::vector<Bone*> children;
};

struct Skelton {
    Bone* rootBone;
    std::unordered_map<std::string, Bone*> boneMap;
    std::vector<aiMatrix4x4> restPoseMatrices;

    void buildRestPoseMatrices();
};

struct Animation {

    struct Channel {
        std::string boneName;
        std::vector<aiVectorKey> positions;
        std::vector<aiQuatKey> rotations;
        std::vector<aiVectorKey> scales;
        aiAnimBehaviour preState;
        aiAnimBehaviour postState;
    };

    std::string name;
    std::vector<Channel> channels;
    double duration;
    double ticksPerSecond;
};

struct Model {
    std::vector<Mesh> meshes;
    Skelton skelton;
    std::vector<Animation> animations;
};

Model* loadModel(const char* path);
Model* loadModelFromMemory(const void* src, size_t len, const char* hint);
void deleteModel(Model* model);

} // namespace kame::assimp
#pragma once

#include <kame/math/math.hpp>
#include <kame/gltf/gltf.hpp>

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

namespace kame::squirtle {

struct AnimationClip {
    struct Channel {
        int targetID = -1;
        enum PathType { kTRANSLATION,
                        kROTATION,
                        kSCALE };
        PathType path;
        uint32_t samplerID;
    };
    struct Sampler {
        enum InterpolationType { kLINEAR,
                                 kSTEP,
                                 kCUBICSPLINE };
        InterpolationType interpolation;
        std::vector<float> inputs;
        std::vector<kame::math::Vector4> outputsVec4;
    };

    std::string name;
    std::vector<Channel> channels;
    std::vector<Sampler> samplers;
    float startTime = std::numeric_limits<float>::max();
    float endTime = std::numeric_limits<float>::min();
};

std::unordered_map<std::string, AnimationClip> importAnimation(const kame::gltf::Gltf* gltf);

} // namespace kame::squirtle

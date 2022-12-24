#pragma once

#include "node.hpp"

#include <vector>
#include <map>
#include <string>
#include <cstdint>

namespace kame::squirtle {

struct Skin {
    std::vector<kame::math::Matrix4x4f> inverseBindMatrices;
    std::vector<uint64_t> joints;
    std::vector<kame::math::Matrix4x4f> skinMatrices;
};

struct AnimationClip {
    struct Channel {
        enum Type {
            kNone,
            kTranslation,
            kRotation,
            kScale,
            kWeights
        };

        uint64_t sampler;
        uint64_t targetNodeID;
        AnimationClip::Channel::Type targetType = AnimationClip::Channel::Type::kNone;
    };
    struct Sampler {
        enum InterpolationType {
            kLINEAR,
            kSTEP,
            kCUBICSPLINE
        };

        InterpolationType interpolationType = AnimationClip::Sampler::InterpolationType::kLINEAR;

        std::vector<float> input;
        std::vector<kame::math::Vector3f> outputVec3;
        std::vector<kame::math::Vector4f> outputVec4;
        double minTime = 0.0;
        double maxTime = 0.0;
    };

    std::string name;

    std::vector<AnimationClip::Channel> channels;
    std::vector<AnimationClip::Sampler> samplers;

    double startTime = 0.0;
    double endTime = 0.0;
};

struct Animation {
    std::vector<kame::squirtle::Node*> nodes;
    std::vector<AnimationClip> clips;
    std::map<std::string, AnimationClip*> clipMap;
};

struct Gltf;

struct AnimationPlayer {
    Animation* animation = nullptr;
    struct GltfNode* gltf = nullptr;
    double currentTime = 0.0;
    bool isLoop = false;
    AnimationClip* clip = nullptr;

    bool hasAnimation()
    {
        return animation && !animation->clips.empty();
    }

    void play(std::string name)
    {
        assert(hasAnimation());
        clip = animation->clipMap[name];
        assert(clip);
        currentTime = 0.0;
        isLoop = false;
    }

    void play()
    {
        assert(hasAnimation());

        clip = &animation->clips.front();
        assert(clip);
        currentTime = 0.0;
        isLoop = false;
    }

    void playLoop(std::string name)
    {
        assert(hasAnimation());
        clip = animation->clipMap[name];
        assert(clip);
        currentTime = 0.0;
        isLoop = true;
    }

    void playLoop()
    {
        assert(hasAnimation());
        clip = &animation->clips.front();
        assert(clip);
        currentTime = 0.0;
        isLoop = true;
    }

    void update(float dt);
};

struct GltfNode : kame::squirtle::Node {

    std::vector<kame::squirtle::Node*> nodes;
    std::vector<Skin>* skins = nullptr;
    AnimationPlayer player;

    SquirtleNodeType getType() const override
    {
        return kSquirtleGltfNode;
    }

    virtual void onUpdate(float dt) override
    {
        player.update(dt);
    }
};

} // namespace kame::squirtle

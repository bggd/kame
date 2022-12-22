#pragma once

#include "node.hpp"

#include <vector>
#include <map>
#include <string>
#include <cstdint>

namespace kame::squirtle {

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

struct AnimationPlayer {
    Animation* animation = nullptr;
    double currentTime = 0.0;
    bool isLoop = false;
    AnimationClip* clip = nullptr;

    bool hasAnimation()
    {
        return animation && !animation->clips.empty();
    }

    void play(std::string name)
    {
        assert(animation);
        clip = animation->clipMap[name];
        assert(clip);
        currentTime = 0.0;
        isLoop = false;
    }

    void play()
    {
        assert(animation);

        clip = &animation->clips.front();
        assert(clip);
        currentTime = 0.0;
        isLoop = false;
    }

    void playLoop(std::string name)
    {
        assert(animation);
        clip = animation->clipMap[name];
        assert(clip);
        currentTime = 0.0;
        isLoop = true;
    }

    void playLoop()
    {
        assert(animation);
        clip = &animation->clips.front();
        assert(clip);
        currentTime = 0.0;
        isLoop = true;
    }

    void update(float dt)
    {
        if (!clip)
        {
            return;
        }

        currentTime += dt;

        if (isLoop)
        {
            double duration = clip->endTime - clip->startTime;
            if (duration <= 0)
            {
                currentTime = 0.0f;
            }
            currentTime = fmod(currentTime - clip->startTime, clip->endTime - clip->startTime);
            if (currentTime < 0.0f)
            {
                currentTime += clip->endTime - clip->startTime;
            }
            currentTime += clip->startTime;
        }
        else
        {
            if (currentTime < clip->startTime)
            {
                currentTime = clip->startTime;
            }
            if (currentTime > clip->endTime)
            {
                currentTime = clip->endTime;
            }
        }

        for (auto& chan : clip->channels)
        {
            auto& spl = clip->samplers[chan.sampler];
            int prevKey = 0;
            for (int i = 0; i < spl.input.size() - 1; ++i)
            {
                prevKey = i;
                if (currentTime < spl.input[i + 1])
                {
                    break;
                }
            }
            double prevTime = spl.input[prevKey];
            double nextTime = spl.input[prevKey + 1];
            double interpolationValue = (currentTime - prevTime) / (nextTime - prevTime);

            if (chan.targetType == AnimationClip::Channel::Type::kTranslation)
            {
                kame::math::Vector3f prevTranslation = spl.outputVec3[prevKey];
                kame::math::Vector3f nextTranslation = spl.outputVec3[prevKey + 1];
                kame::math::Vector3f translation = kame::math::Vector3f::lerp(prevTranslation, nextTranslation, interpolationValue);
                animation->nodes[chan.targetNodeID]->setLocation(translation);
            }
            else if (chan.targetType == AnimationClip::Channel::Type::kRotation)
            {
                kame::math::Quaternion prevRotation = spl.outputVec4[prevKey];
                kame::math::Quaternion nextRotation = spl.outputVec4[prevKey + 1];
                kame::math::Quaternion rotation = kame::math::Quaternion::slerp(prevRotation, nextRotation, interpolationValue);
                auto* node = animation->nodes[chan.targetNodeID];
                node->rotationMode = kSquirtleRotationModeQuaternion;
                node->setQuatRotation(rotation);
            }
        }
    }
};

struct GltfNode : kame::squirtle::Node {

    Animation* animation;
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

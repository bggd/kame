#include <all.hpp>

namespace kame::squirtle {

void AnimationPlayer::update(float dt)
{
    if (!clip)
    {
        return;
    }
    assert(gltf);

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
            gltf->nodes[chan.targetNodeID]->setLocation(translation);
        }
        else if (chan.targetType == AnimationClip::Channel::Type::kRotation)
        {
            kame::math::Quaternion prevRotation = spl.outputVec4[prevKey];
            kame::math::Quaternion nextRotation = spl.outputVec4[prevKey + 1];
            kame::math::Quaternion rotation = kame::math::Quaternion::slerp(prevRotation, nextRotation, interpolationValue);
            auto* node = gltf->nodes[chan.targetNodeID];
            node->rotationMode = kSquirtleRotationModeQuaternion;
            node->setQuatRotation(rotation);
        }
    }
}

} // namespace kame::squirtle

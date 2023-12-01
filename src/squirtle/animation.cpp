#include <all.hpp>

namespace kame::squirtle {

std::unordered_map<std::string, AnimationClip> importAnimation(const kame::gltf::Gltf* gltf)
{
    std::unordered_map<std::string, AnimationClip> clips;

    clips.reserve(gltf->animations.size());
    for (auto& a : gltf->animations)
    {
        assert(clips.contains(a.name) == false);
        AnimationClip& clip = clips[a.name];
        clip.name = a.name;
        clip.channels.reserve(a.channels.size());
        for (auto& c : a.channels)
        {
            clip.channels.emplace_back();
            AnimationClip::Channel& chan = clip.channels.back();
            chan.samplerID = c.sampler;
            if (c.target.hasNode)
            {
                chan.targetID = c.target.node;
            }
            if (c.target.path == "translation")
            {
                chan.path = AnimationClip::Channel::PathType::kTRANSLATION;
            }
            else if (c.target.path == "rotation")
            {
                chan.path = AnimationClip::Channel::PathType::kROTATION;
            }
            else if (c.target.path == "scale")
            {
                chan.path = AnimationClip::Channel::PathType::kSCALE;
            }
        }
        clip.samplers.reserve(a.samplers.size());
        for (auto& s : a.samplers)
        {
            clip.samplers.emplace_back();
            AnimationClip::Sampler& smp = clip.samplers.back();
            if (s.interpolation == "LINEAR")
            {
                smp.interpolation = AnimationClip::Sampler::InterpolationType::kLINEAR;
            }
            else if (s.interpolation == "STEP")
            {
                smp.interpolation = AnimationClip::Sampler::InterpolationType::kSTEP;
            }
            else if (s.interpolation == "CUBICSPLINE")
            {
                smp.interpolation = AnimationClip::Sampler::InterpolationType::kCUBICSPLINE;
            }

            {
                auto& acc = gltf->accessors[s.input];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                smp.inputs.reserve(acc.count);
                assert(acc.componentType == GL_FLOAT);
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    auto v = ((float*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    smp.inputs.emplace_back(v);
                    clip.startTime = std::min(clip.startTime, v);
                    clip.endTime = std::max(clip.endTime, v);
                }
            }
            {
                auto& acc = gltf->accessors[s.output];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                smp.outputsVec4.reserve(acc.count);
                assert(acc.componentType == GL_FLOAT);
                if (acc.type == "VEC3")
                {
                    for (unsigned int i = 0; i < acc.count; ++i)
                    {
                        auto v = ((kame::math::Vector3*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        smp.outputsVec4.emplace_back(kame::math::Vector4(v, 0.0f));
                    }
                }
                else if (acc.type == "VEC4")
                {
                    for (unsigned int i = 0; i < acc.count; ++i)
                    {
                        auto v = ((kame::math::Vector4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        smp.outputsVec4.emplace_back(v);
                    }
                }
            }
        }
    }

    return clips;
}

} // namespace kame::squirtle

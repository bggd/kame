#include <all.hpp>

namespace kame::squirtle {

InstanceAttributes importInstanceAttributes(const kame::gltf::Gltf* gltf)
{
    InstanceAttributes instance;
    for (const auto& node : gltf->nodes)
    {
        if (!node.hasExtensions)
        {
            continue;
        }
        if (!node.extensions->hasEXT_mesh_gpu_instancing)
        {
            continue;
        }

        for (auto& item : node.extensions->EXT_mesh_gpu_instancing->attributes)
        {
            if (item.first == "TRANSLATION")
            {
                auto& acc = gltf->accessors[item.second];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                instance.tranlations.reserve(acc.count);
                assert(acc.componentType == GL_FLOAT);
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    auto v = ((kame::math::Vector3*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    instance.tranlations.emplace_back(v);
                }
            }
            else if (item.first == "ROTATION")
            {
                auto& acc = gltf->accessors[item.second];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                instance.rotations.reserve(acc.count);
                assert(acc.componentType == GL_FLOAT);
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    auto v = ((kame::math::Vector4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    instance.rotations.emplace_back(v);
                }
            }
            else if (item.first == "SCALE")
            {
                auto& acc = gltf->accessors[item.second];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                instance.scales.reserve(acc.count);
                assert(acc.componentType == GL_FLOAT);
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    auto v = ((float*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    instance.scales.emplace_back(v);
                }
            }
        }
        break;
    }
    assert(instance.tranlations.size() == instance.rotations.size());
    assert(instance.tranlations.size() == instance.scales.size());
    return instance;
}

std::vector<kame::math::Matrix> toInstanceMatrices(const InstanceAttributes& attributes)
{
    std::vector<kame::math::Matrix> instances;
    assert(attributes.tranlations.size() == attributes.rotations.size() && attributes.tranlations.size() == attributes.scales.size());
    instances.reserve(attributes.tranlations.size());
    for (size_t i = 0; i < attributes.tranlations.size(); ++i)
    {
        auto pos = attributes.tranlations[i];
        auto rot = attributes.rotations[i];
        auto scale = attributes.scales[i];
        auto mtx = kame::math::Matrix::createScale(scale) * kame::math::Matrix::createFromQuaternion(rot) * kame::math::Matrix::createTranslation(pos);
        instances.emplace_back(mtx);
    }
    return instances;
}

} // namespace kame::squirtle

#include <all.hpp>

namespace kame::squirtle {

std::vector<kame::math::Vector3> toVertexPositions(const kame::gltf::Gltf* gltf, const kame::gltf::Mesh& m)
{
    std::vector<kame::math::Vector3> positions;

    for (auto& pri : m.primitives)
    {
        for (auto& item : pri.attributes)
        {
            if (item.first == "POSITION")
            {
                auto& acc = gltf->accessors[item.second];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                positions.reserve(acc.count);
                assert(acc.componentType == GL_FLOAT);
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    auto v = ((kame::math::Vector3*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    positions.emplace_back(v);
                }
            }
        }
    }

    return positions;
}

std::vector<std::vector<kame::math::Vector2>> toVertexUVSets(const kame::gltf::Gltf* gltf, const kame::gltf::Mesh& m)
{
    std::vector<std::vector<kame::math::Vector2>> uvSets;

    for (auto& pri : m.primitives)
    {
        for (auto& item : pri.attributes)
        {
            if (pystring::startswith(item.first, "TEXCOORD"))
            {
                uvSets.emplace_back();
                int uvid = stoi(pystring::lstrip(item.first, "TEXCOORD_"));
                assert(uvid >= 0);
                assert(size_t(uvid) < uvSets.size());

                auto& acc = gltf->accessors[item.second];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                uvSets[uvid].reserve(acc.count);
                assert(acc.componentType == GL_FLOAT || acc.componentType == GL_UNSIGNED_BYTE || acc.componentType == GL_UNSIGNED_SHORT);
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    if (acc.componentType == GL_FLOAT)
                    {
                        auto v = ((kame::math::Vector2*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        uvSets[uvid].emplace_back(v);
                    }
                    else if (acc.componentType == GL_UNSIGNED_BYTE)
                    {
                        auto e = ((unsigned char*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        uvSets[uvid].emplace_back(e / 255.0f);
                    }
                    else if (acc.componentType == GL_UNSIGNED_SHORT)
                    {
                        auto e = ((unsigned short*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        uvSets[uvid].emplace_back(e / 65535.0f);
                    }
                }
            }
        }
    }

    return uvSets;
}

std::vector<u16Array4> toVertexJoints(const kame::gltf::Gltf* gltf, const kame::gltf::Mesh& m)
{
    std::vector<std::array<uint16_t, 4>> joints;

    for (auto& pri : m.primitives)
    {
        for (auto& item : pri.attributes)
        {
            if (item.first == "JOINTS_0")
            {
                auto& acc = gltf->accessors[item.second];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                joints.reserve(acc.count);
                assert(acc.componentType == GL_UNSIGNED_BYTE || acc.componentType == GL_UNSIGNED_SHORT);
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    if (acc.componentType == GL_UNSIGNED_BYTE)
                    {
                        auto e = ((u8Array4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        u16Array4 v;
                        v[0] = e[0];
                        v[1] = e[1];
                        v[2] = e[2];
                        v[3] = e[3];
                        joints.emplace_back(v);
                    }
                    else if (acc.componentType == GL_UNSIGNED_SHORT)
                    {
                        auto v = ((u16Array4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        joints.emplace_back(v);
                    }
                }
            }
        }
    }

    return joints;
}

std::vector<kame::math::Vector4> toVertexWeights(const kame::gltf::Gltf* gltf, const kame::gltf::Mesh& m)
{
    std::vector<kame::math::Vector4> weights;

    for (auto& pri : m.primitives)
    {
        for (auto& item : pri.attributes)
        {
            if (item.first == "WEIGHTS_0")
            {
                auto& acc = gltf->accessors[item.second];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                weights.reserve(acc.count);
                assert(acc.componentType == GL_FLOAT || acc.componentType == GL_UNSIGNED_BYTE || acc.componentType == GL_UNSIGNED_SHORT);
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    if (acc.componentType == GL_FLOAT)
                    {
                        auto v = ((kame::math::Vector4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        weights.emplace_back(v);
                    }
                    else if (acc.componentType == GL_UNSIGNED_BYTE)
                    {
                        auto e = ((u8Array4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        kame::math::Vector4 v;
                        v.x = e[0] / 255.0f;
                        v.y = e[1] / 255.0f;
                        v.z = e[2] / 255.0f;
                        v.w = e[3] / 255.0f;
                        weights.emplace_back(v);
                    }
                    else if (acc.componentType == GL_UNSIGNED_SHORT)
                    {
                        auto e = ((u16Array4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        kame::math::Vector4 v;
                        v.x = e[0] / 65535.0f;
                        v.y = e[1] / 65535.0f;
                        v.z = e[2] / 65535.0f;
                        v.w = e[3] / 65535.0f;
                        weights.emplace_back(v);
                    }
                }
            }
        }
    }

    return weights;
}

std::vector<unsigned int> toVertexIndices(const kame::gltf::Gltf* gltf, const kame::gltf::Mesh& m)
{
    std::vector<unsigned int> indices;

    for (auto& pri : m.primitives)
    {
        if (pri.hasIndices)
        {
            auto& acc = gltf->accessors[pri.indices];
            auto& bv = gltf->bufferViews[acc.bufferView];
            auto& b = gltf->buffers[bv.buffer];
            indices.reserve(acc.count);
            assert(acc.componentType == GL_UNSIGNED_INT || acc.componentType == GL_UNSIGNED_BYTE || acc.componentType == GL_UNSIGNED_SHORT);
            for (unsigned int i = 0; i < acc.count; ++i)
            {
                if (acc.componentType == GL_UNSIGNED_BYTE)
                {
                    auto e = ((unsigned char*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    indices.emplace_back(e);
                }
                else if (acc.componentType == GL_UNSIGNED_SHORT)
                {
                    auto e = ((unsigned short*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    indices.emplace_back(e);
                }
                else if (acc.componentType == GL_UNSIGNED_INT)
                {
                    auto e = ((unsigned int*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    indices.emplace_back(e);
                }
            }
        }
    }

    return indices;
}

Model* importModel(const kame::gltf::Gltf* gltf)
{
    Model* model = new Model();
    assert(model);
    model->meshes.reserve(gltf->meshes.size());
    for (auto& m : gltf->meshes)
    {
        model->meshes.emplace_back();
        auto& mesh = model->meshes.back();

        mesh.positions = toVertexPositions(gltf, m);
        mesh.uvSets = toVertexUVSets(gltf, m);
        mesh.joints = toVertexJoints(gltf, m);
        mesh.weights = toVertexWeights(gltf, m);
        mesh.indices = toVertexIndices(gltf, m);
    }
    model->nodes.resize(gltf->nodes.size());
    size_t nodeID = 0;
    for (auto& n : gltf->nodes)
    {
        Node& node = model->nodes[nodeID];
        if (n.hasMesh)
        {
            node.meshID = n.mesh;
        }
        if (n.hasSkin)
        {
            node.skinID = n.skin;
            model->_isSkinnedMesh = true;
        }
        if (n.hasTranslation)
        {
            node.position.x = n.translation[0];
            node.position.y = n.translation[1];
            node.position.z = n.translation[2];
        }
        if (n.hasRotation)
        {
            node.rotation.x = n.rotation[0];
            node.rotation.y = n.rotation[1];
            node.rotation.z = n.rotation[2];
            node.rotation.w = n.rotation[3];
        }
        if (n.hasScale)
        {
            node.scale.x = n.scale[0];
            node.scale.y = n.scale[1];
            node.scale.z = n.scale[2];
        }
        if (n.hasMatrix)
        {
            // decompose a Matrix to TRS. code from https://github.com/toji/gl-matrix and https://github.com/donmccurdy/glTF-Transform
            kame::math::Matrix srcMat;
            std::memcpy(&srcMat, n.matrix, sizeof(kame::math::Matrix));
            float sx = kame::math::Vector3::length(kame::math::Vector3(srcMat.m11, srcMat.m12, srcMat.m13));
            float sy = kame::math::Vector3::length(kame::math::Vector3(srcMat.m21, srcMat.m22, srcMat.m23));
            float sz = kame::math::Vector3::length(kame::math::Vector3(srcMat.m31, srcMat.m32, srcMat.m33));

            if (srcMat.determinant() < 0.0f)
            {
                sx = -sx;
            }

            node.position.x = srcMat.m41;
            node.position.y = srcMat.m42;
            node.position.z = srcMat.m43;

            float invSX = 1.0f / sx;
            float invSY = 1.0f / sy;
            float invSZ = 1.0f / sz;

            srcMat.m11 *= invSX;
            srcMat.m12 *= invSX;
            srcMat.m13 *= invSX;

            srcMat.m21 *= invSY;
            srcMat.m22 *= invSY;
            srcMat.m23 *= invSY;

            srcMat.m31 *= invSZ;
            srcMat.m32 *= invSZ;
            srcMat.m33 *= invSZ;

            float is1 = 1.0f / sx;
            float is2 = 1.0f / sy;
            float is3 = 1.0f / sz;

            float sm11 = srcMat.m11 * is1;
            float sm12 = srcMat.m12 * is2;
            float sm13 = srcMat.m13 * is3;
            float sm21 = srcMat.m21 * is1;
            float sm22 = srcMat.m22 * is2;
            float sm23 = srcMat.m23 * is3;
            float sm31 = srcMat.m31 * is1;
            float sm32 = srcMat.m32 * is2;
            float sm33 = srcMat.m33 * is3;

            float trace = sm11 + sm22 + sm33;
            float S = 0;

            if (trace > 0)
            {
                S = sqrtf(trace + 1.0) * 2;
                node.rotation.w = 0.25 * S;
                node.rotation.x = (sm23 - sm32) / S;
                node.rotation.y = (sm31 - sm13) / S;
                node.rotation.z = (sm12 - sm21) / S;
            }
            else if (sm11 > sm22 && sm11 > sm33)
            {
                S = sqrtf(1.0 + sm11 - sm22 - sm33) * 2;
                node.rotation.w = (sm23 - sm32) / S;
                node.rotation.x = 0.25 * S;
                node.rotation.y = (sm12 + sm21) / S;
                node.rotation.z = (sm31 + sm13) / S;
            }
            else if (sm22 > sm33)
            {
                S = sqrtf(1.0 + sm22 - sm11 - sm33) * 2;
                node.rotation.w = (sm31 - sm13) / S;
                node.rotation.x = (sm12 + sm21) / S;
                node.rotation.y = 0.25 * S;
                node.rotation.z = (sm23 + sm32) / S;
            }
            else
            {
                S = sqrtf(1.0 + sm33 - sm11 - sm22) * 2;
                node.rotation.w = (sm12 - sm21) / S;
                node.rotation.x = (sm31 + sm13) / S;
                node.rotation.y = (sm23 + sm32) / S;
                node.rotation.z = 0.25 * S;
            }

            node.scale.x = sx;
            node.scale.y = sy;
            node.scale.z = sz;
        }
        node.children.reserve(n.children.size());
        for (auto c : n.children)
        {
            model->nodes[c].parent = nodeID;
            node.children.push_back(c);
        }
        ++nodeID;
    }

    model->clips.reserve(gltf->animations.size());
    for (auto& a : gltf->animations)
    {
        assert(model->clips.contains(a.name) == false);
        AnimationClip& clip = model->clips[a.name];
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
                chan.path = AnimationClip::Channel::PathType::TRANSLATION;
            }
            else if (c.target.path == "rotation")
            {
                chan.path = AnimationClip::Channel::PathType::ROTATION;
            }
            else if (c.target.path == "scale")
            {
                chan.path = AnimationClip::Channel::PathType::SCALE;
            }
        }
        clip.samplers.reserve(a.samplers.size());
        for (auto& s : a.samplers)
        {
            clip.samplers.emplace_back();
            AnimationClip::Sampler& smp = clip.samplers.back();
            if (s.interpolation == "LINEAR")
            {
                smp.interpolation = AnimationClip::Sampler::InterpolationType::LINEAR;
            }
            else if (s.interpolation == "STEP")
            {
                smp.interpolation = AnimationClip::Sampler::InterpolationType::STEP;
            }
            else if (s.interpolation == "CUBICSPLINE")
            {
                smp.interpolation = AnimationClip::Sampler::InterpolationType::CUBICSPLINE;
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

    model->skins.reserve(gltf->skins.size());
    for (auto& s : gltf->skins)
    {
        model->skins.emplace_back();
        auto& skin = model->skins.back();
        assert(s.hasInverseBindMatrices);
        auto& acc = gltf->accessors[s.inverseBindMatrices];
        auto& bv = gltf->bufferViews[acc.bufferView];
        auto& b = gltf->buffers[bv.buffer];
        skin.inverseBindMatrices.reserve(acc.count);
        assert(acc.componentType == GL_FLOAT);
        for (unsigned int i = 0; i < acc.count; ++i)
        {
            auto v = ((kame::math::Matrix*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
            skin.inverseBindMatrices.emplace_back(v);
        }
        skin.joints.reserve(s.joints.size());
        for (auto& jID : s.joints)
        {
            skin.joints.emplace_back(jID);
        }
        skin.matrices.resize(s.joints.size());
    }

    return model;
}

void updateGlobalXForm(Model* model, int id)
{
    Node& node = model->nodes[id];
    auto local = node.updateLocalXForm();
    auto global = kame::math::Matrix::identity();

    if (node.parent >= 0 && node.skinID < 0)
    {
        global = model->nodes[node.parent].globalXForm;
    }

    node.globalXForm = local * global;
    for (auto c : node.children)
    {
        updateGlobalXForm(model, c);
    }
}

void updateSkinMatrices(Model* model)
{
    for (auto& skin : model->skins)
    {
        for (uint32_t i = 0; i < skin.joints.size(); ++i)
        {
            Node& joint = model->nodes[skin.joints[i]];
            skin.matrices[i] = skin.inverseBindMatrices[i] * joint.globalXForm;
        }
    }
}

void updateSkinnedMesh(Model* model, std::vector<kame::math::Vector3>& positions)
{
    size_t offset = 0;
    for (auto& n : model->nodes)
    {
        if (n.meshID < 0)
        {
            continue;
        }

        auto invertMtx = kame::math::Matrix::invert(n.globalXForm);
        if (n.skinID >= 0)
        {
            Skin& s = model->skins[n.skinID];
            static std::vector<kame::math::Matrix> skinMatrices;
            skinMatrices.resize(s.matrices.size());
            for (size_t i = 0; i < s.matrices.size(); ++i)
            {
                skinMatrices[i] = s.matrices[i] * invertMtx;
            }

            Mesh& srcMesh = model->meshes[n.meshID];
            for (auto i : srcMesh.indices)
            {
                auto vPos = srcMesh.positions[i];
                auto vJoint = srcMesh.joints[i];
                auto vWeight = srcMesh.weights[i];

                // clang-format off
                auto skinMtx =
                      skinMatrices[vJoint[0]] * vWeight.x
                    + skinMatrices[vJoint[1]] * vWeight.y
                    + skinMatrices[vJoint[2]] * vWeight.z
                    + skinMatrices[vJoint[3]] * vWeight.w;
                // clang-format on

                positions[offset + i] = kame::math::Vector3::transform(vPos, skinMtx);
            }
            offset += srcMesh.positions.size();
        }
    }
}

void updateMesh(Model* model, std::vector<kame::math::Vector3>& positions)
{
    size_t offset = 0;
    for (auto& n : model->nodes)
    {
        if (n.meshID < 0)
        {
            continue;
        }

        Mesh& srcMesh = model->meshes[n.meshID];
        for (auto i : srcMesh.indices)
        {
            auto vPos = srcMesh.positions[i];
            positions[offset + i] = kame::math::Vector3::transform(vPos, n.globalXForm);
        }

        offset += srcMesh.positions.size();
    }
}

void Model::setAnimationClip(std::string name)
{
    auto it = clips.find(name);
    assert(it != clips.end());
    activeClip = &it->second;

    playTime = activeClip->startTime;
    isPlay = false;
}

void Model::playAnimation()
{
    assert(activeClip);
    isPlay = true;
}

void Model::updateAnimation(float dt)
{
    if (!activeClip || !isPlay)
    {
        return;
    }

    playTime += dt;
    if (playTime > activeClip->endTime)
    {
        playTime = activeClip->startTime + (activeClip->endTime - playTime);
    }

    AnimationClip& clip = *activeClip;
    float time = playTime;

    for (auto& c : clip.channels)
    {
        if (c.targetID < 0)
        {
            continue;
        }

        Node& node = nodes[c.targetID];

        auto& s = clip.samplers[c.samplerID];
        if (s.inputs.size() > s.outputsVec4.size())
        {
            continue;
        }

        for (size_t i = 0; i < s.outputsVec4.size() - 1; ++i)
        {
            if ((time >= s.inputs[i]) && (time <= s.inputs[i + 1]))
            {
                float u = std::max(0.0f, time - s.inputs[i]) / (s.inputs[i + 1] - s.inputs[i]);
                if (u <= 1.0f)
                {
                    // TODO: STEP and CUBICSPLINE interp
                    switch (c.path)
                    {
                        case AnimationClip::Channel::PathType::TRANSLATION: {
                            auto trans = kame::math::Vector4::lerp(s.outputsVec4[i], s.outputsVec4[i + 1], u);
                            node.position = kame::math::Vector3(trans.x, trans.y, trans.z);
                            break;
                        }
                        case AnimationClip::Channel::PathType::SCALE: {
                            auto scale = kame::math::Vector4::lerp(s.outputsVec4[i], s.outputsVec4[i + 1], u);
                            node.scale = kame::math::Vector3(scale.x, scale.y, scale.z);
                            break;
                        }
                        case AnimationClip::Channel::PathType::ROTATION: {
                            auto rot = kame::math::Quaternion::slerp(s.outputsVec4[i], s.outputsVec4[i + 1], u);
                            node.rotation = kame::math::Quaternion::normalize(rot);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void Model::prepareDraw(std::vector<kame::math::Vector3>& positions)
{
    if (!nodes.empty())
    {
        int i = 0;
        for (auto& n : nodes)
        {
            if (n.parent < 0)
            {
                updateGlobalXForm(this, i);
            }
            ++i;
        }
    }

    if (isSkinnedMesh())
    {
        updateSkinMatrices(this);
        updateSkinnedMesh(this, positions);
    }
    else
    {
        updateMesh(this, positions);
    }
}

} // namespace kame::squirtle

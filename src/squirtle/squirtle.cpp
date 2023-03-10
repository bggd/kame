#include <all.hpp>

namespace kame::squirtle {

void VBOMesh::initVBOMesh(const Mesh& mesh)
{
    vboPositions = kame::ogl::createVertexBuffer(mesh.positions.size() * 3 * sizeof(float), GL_STATIC_DRAW);
    vboPositions->setBuffer((const float*)&mesh.positions[0]);

    vboUVSets.reserve(mesh.uvSets.size());
    for (const auto& uv : mesh.uvSets)
    {
        auto* vboTexcoords = kame::ogl::createVertexBuffer(uv.size() * 2 * sizeof(float), GL_STATIC_DRAW);
        vboTexcoords->setBuffer((const float*)&uv[0]);
        vboUVSets.push_back(vboTexcoords);
    }

    iboIndices = kame::ogl::createIndexBuffer(mesh.indices.size() * sizeof(unsigned int), GL_STATIC_DRAW);
    iboIndices->setBuffer((const unsigned int*)&mesh.indices[0]);
    numIndex = mesh.indices.size();
}

void VBOMesh::shutdownVBOMesh()
{
    kame::ogl::deleteIndexBuffer(iboIndices);
    for (auto& uv : vboUVSets)
    {
        kame::ogl::deleteVertexBuffer(uv);
    }
    kame::ogl::deleteVertexBuffer(vboPositions);
}

void cleanupVBOMeshes(std::vector<VBOMesh>& vboMeshes)
{
    for (auto& vbo : vboMeshes)
    {
        vbo.shutdownVBOMesh();
    }
}

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
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    auto v = ((kame::math::Vector3*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    positions.push_back(v);
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
                assert(uvid < uvSets.size());

                auto& acc = gltf->accessors[item.second];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                uvSets[uvid].reserve(acc.count);
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    if (acc.componentType == GL_FLOAT)
                    {
                        auto v = ((kame::math::Vector2*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        uvSets[uvid].push_back(v);
                    }
                    else if (acc.componentType == GL_UNSIGNED_BYTE)
                    {
                        auto e = ((unsigned char*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        uvSets[uvid].push_back(e / 255.0f);
                    }
                    else if (acc.componentType == GL_UNSIGNED_SHORT)
                    {
                        auto e = ((unsigned short*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        uvSets[uvid].push_back(e / 65535.0f);
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
                        joints.push_back(v);
                    }
                    else if (acc.componentType == GL_UNSIGNED_SHORT)
                    {
                        auto v = ((u16Array4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        joints.push_back(v);
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
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    if (acc.componentType == GL_FLOAT)
                    {
                        auto v = ((kame::math::Vector4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        weights.push_back(v);
                    }
                    else if (acc.componentType == GL_UNSIGNED_BYTE)
                    {
                        auto e = ((u8Array4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        kame::math::Vector4 v;
                        v.x = e[0] / 255.0f;
                        v.y = e[1] / 255.0f;
                        v.z = e[2] / 255.0f;
                        v.w = e[3] / 255.0f;
                        weights.push_back(v);
                    }
                    else if (acc.componentType == GL_UNSIGNED_SHORT)
                    {
                        auto e = ((u16Array4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        kame::math::Vector4 v;
                        v.x = e[0] / 65535.0f;
                        v.y = e[1] / 65535.0f;
                        v.z = e[2] / 65535.0f;
                        v.w = e[3] / 65535.0f;
                        weights.push_back(v);
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
            for (unsigned int i = 0; i < acc.count; ++i)
            {
                if (acc.componentType == GL_UNSIGNED_BYTE)
                {
                    auto e = ((unsigned char*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    indices.push_back(e);
                }
                else if (acc.componentType == GL_UNSIGNED_SHORT)
                {
                    auto e = ((unsigned short*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    indices.push_back(e);
                }
                else if (acc.componentType == GL_UNSIGNED_INT)
                {
                    auto e = ((unsigned int*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                    indices.push_back(e);
                }
            }
        }
    }

    return indices;
}

Model importModel(const kame::gltf::Gltf* gltf)
{
    Model model;
    model.meshes.reserve(gltf->meshes.size());
    model.vboMeshes.reserve(gltf->meshes.size());
    for (auto& m : gltf->meshes)
    {
        model.meshes.emplace_back();
        auto& mesh = model.meshes.back();

        mesh.positions = toVertexPositions(gltf, m);
        mesh.uvSets = toVertexUVSets(gltf, m);
        mesh.joints = toVertexJoints(gltf, m);
        mesh.weights = toVertexWeights(gltf, m);
        mesh.indices = toVertexIndices(gltf, m);

        model.vboMeshes.emplace_back();
        auto& vbo = model.vboMeshes.back();
        vbo.initVBOMesh(mesh);
    }
    model.nodes.resize(gltf->nodes.size() + 1);
    int nodeID = 0;
    for (auto& n : gltf->nodes)
    {
        Node& node = model.nodes[nodeID];
        if (n.hasMesh)
        {
            node.meshID = n.mesh;
        }
        if (n.hasSkin)
        {
            node.skinID = n.skin;
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
        node.children.reserve(n.children.size());
        for (auto c : n.children)
        {
            model.nodes[c].parent = nodeID;
            node.children.push_back(c);
        }
        ++nodeID;
    }
    nodeID = 0;
    for (auto& node : model.nodes)
    {
        if (nodeID >= gltf->nodes.size())
        {
            break;
        }
        if (node.parent < 0)
        {
            auto& root = model.nodes.back();
            root.children.push_back(nodeID);
        }
        ++nodeID;
    }
    model.clips.reserve(gltf->animations.size());
    for (auto& a : gltf->animations)
    {
        model.clips.emplace_back();
        AnimationClip& clip = model.clips.back();
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
                    smp.inputs.push_back(v);
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
                        smp.outputsVec4.push_back(kame::math::Vector4(v, 0.0f));
                    }
                }
                else if (acc.type == "VEC4")
                {
                    for (unsigned int i = 0; i < acc.count; ++i)
                    {
                        auto v = ((kame::math::Vector4*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
                        smp.outputsVec4.push_back(v);
                    }
                }
            }
        }
    }
    model.skins.reserve(gltf->skins.size());
    for (auto& s : gltf->skins)
    {
        model.skins.emplace_back();
        auto& skin = model.skins.back();
        assert(s.hasInverseBindMatrices);
        auto& acc = gltf->accessors[s.inverseBindMatrices];
        auto& bv = gltf->bufferViews[acc.bufferView];
        auto& b = gltf->buffers[bv.buffer];
        skin.inverseBindMatrices.reserve(acc.count);
        for (unsigned int i = 0; i < acc.count; ++i)
        {
            auto v = ((kame::math::Matrix*)(b.data() + bv.byteOffset + acc.byteOffset))[i];
            skin.inverseBindMatrices.push_back(v);
        }
        skin.joints.reserve(s.joints.size());
        for (auto& jID : s.joints)
        {
            skin.joints.push_back(jID);
        }
        skin.matrices.resize(s.joints.size());
    }

    return model;
}

void updateAnimation(Model& model, std::vector<Node>& nodes, AnimationClip& clip, float time)
{
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
                    model.animationUpdated = true;
                }
            }
        }
    }
}

void updateGlobalXForm(Model& model, int id)
{
    Node& node = model.nodes[id];
    auto local = node.updateLocalXForm();
    auto global = kame::math::Matrix::identity();
    if (node.parent >= 0)
    {
        global = model.nodes[node.parent].globalXForm;
    }
    else
    {
        global = model.nodes.back().globalXForm;
    }
    node.globalXForm = local * global;
    for (auto c : node.children)
    {
        updateGlobalXForm(model, c);
    }
}

void updateSkinMatrices(Model& model)
{
    for (auto& skin : model.skins)
    {
        for (uint32_t i = 0; i < skin.joints.size(); ++i)
        {
            Node& joint = model.nodes[skin.joints[i]];
            skin.matrices[i] = skin.inverseBindMatrices[i] * joint.globalXForm;
        }
    }
}

} // namespace kame::squirtle

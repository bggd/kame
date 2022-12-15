#include <all.hpp>

void loadMesh(kame::squirtle::MeshNode* meshNode, const kame::gltf::Gltf* gltf, const kame::gltf::Node& gltfNode)
{
    std::vector<kame::math::Vector3f> positions;
    std::vector<kame::math::Vector2f> texcoords;
    std::vector<kame::math::Vector3f> normals;
    std::vector<uint32_t> indices;

    auto& m = gltf->meshes[gltfNode.mesh];

    for (auto& pri : m.primitives)
    {
        assert(pri.mode == 4); // require TRIANGLES == mesh.primitive.mode
        if (pri.hasIndices)
        {
            auto& acc = gltf->accessors[pri.indices];
            auto& bv = gltf->bufferViews[acc.bufferView];
            auto& b = gltf->buffers[bv.buffer];
            for (unsigned int i = 0; i < acc.count; ++i)
            {
                if (acc.componentType == GL_UNSIGNED_BYTE)
                {
                    auto e = ((unsigned char*)(b.decodedData.data() + bv.byteOffset + acc.byteOffset))[i];
                    indices.push_back(e);
                }
                else if (acc.componentType == GL_UNSIGNED_SHORT)
                {
                    auto e = ((unsigned short*)(b.decodedData.data() + bv.byteOffset + acc.byteOffset))[i];
                    indices.push_back(e);
                }
                else if (acc.componentType == GL_UNSIGNED_INT)
                {
                    auto e = ((unsigned int*)(b.decodedData.data() + bv.byteOffset + acc.byteOffset))[i];
                    indices.push_back(e);
                }
            }
        }
        for (auto& item : pri.attributes)
        {
            if (item.first == "POSITION")
            {
                auto& acc = gltf->accessors[item.second];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    auto v = ((kame::math::Vector3f*)(b.decodedData.data() + bv.byteOffset + acc.byteOffset))[i];
                    positions.push_back(v);
                }
            }
            else if (item.first == "TEXCOORD_0")
            {
                auto& acc = gltf->accessors[item.second];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                assert(acc.componentType == GL_FLOAT);
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    // TODO: support GL_UNSIGNED_BYTE and GL_UNSIGNED_SHORT
                    kame::math::Vector2f v = ((kame::math::Vector2f*)(b.decodedData.data() + bv.byteOffset + acc.byteOffset))[i];
                    texcoords.push_back(v);
                }
            }
            else if (item.first == "NORMAL")
            {
                auto& acc = gltf->accessors[item.second];
                auto& bv = gltf->bufferViews[acc.bufferView];
                auto& b = gltf->buffers[bv.buffer];
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    auto v = ((kame::math::Vector3f*)(b.decodedData.data() + bv.byteOffset + acc.byteOffset))[i];
                    normals.push_back(v);
                }
            }
        }
    }

    meshNode->mesh->positions = std::move(positions);
    meshNode->mesh->texCoords = std::move(texcoords);
    meshNode->mesh->normals = std::move(normals);
    meshNode->mesh->indices = std::move(indices);

    meshNode->bufferedVBO.init(meshNode->mesh);
}

void loadNode(kame::squirtle::Node* parent, const kame::gltf::Gltf* gltf, kame::gltf::integer gltfNodeID, std::vector<kame::squirtle::Node*>& squirtleNodes)
{
    auto& gltfNode = gltf->nodes[gltfNodeID];

    kame::squirtle::Node* self = nullptr;

    if (gltfNode.hasMesh)
    {
        kame::squirtle::MeshNode* meshNode = new kame::squirtle::MeshNode();
        assert(meshNode);
        meshNode->mesh = new kame::squirtle::Mesh();
        assert(meshNode->mesh);

        loadMesh(meshNode, gltf, gltfNode);

        self = meshNode;
    }
    else
    {
        self = new kame::squirtle::Node();
        assert(self);
    }

    if (gltfNode.hasMatrix)
    {
        float* local = (float*)&self->localTransform;
        float* m = (float*)&gltfNode.matrix;
        for (int i = 0; i < 16; ++i)
        {
            local[i] = m[i];
        }
    }
    if (gltfNode.hasTranslation)
    {
        self->setLocation(gltfNode.translation[0], gltfNode.translation[1], gltfNode.translation[2]);
    }
    if (gltfNode.hasRotation)
    {
        self->rotationMode = kSquirtleRotationModeQuaternion;
        self->setQuatRotation(gltfNode.rotation[0], gltfNode.rotation[1], gltfNode.rotation[2], gltfNode.rotation[3]);
    }

    parent->addChild(self);
    squirtleNodes[gltfNodeID] = self;

    for (auto childID : gltfNode.children)
    {
        loadNode(self, gltf, childID, squirtleNodes);
    }
}

kame::squirtle::AnimationClip loadAnimation(std::vector<kame::squirtle::Node*>& squirtleNodes, const kame::gltf::Animation& gltfAnimation, const kame::gltf::Gltf* gltf)
{
    using namespace kame::squirtle;
    kame::squirtle::AnimationClip clip;

    clip.name = gltfAnimation.name;

    double minTime = 0.0;
    double maxTime = 0.0;

    for (auto& c : gltfAnimation.channels)
    {
        kame::squirtle::AnimationClip::Channel chan;

        chan.sampler = c.sampler;

        assert(c.target.hasNode);
        chan.targetNodeID = c.target.node;

        if (c.target.path == "translation")
        {
            chan.targetType = AnimationClip::Channel::Channel::Type::kTranslation;
        }
        else if (c.target.path == "rotation")
        {
            chan.targetType = AnimationClip::Channel::Channel::Type::kRotation;
        }
        else if (c.target.path == "scale")
        {
            chan.targetType = AnimationClip::Channel::Channel::Type::kScale;
        }
        else if (c.target.path == "weights")
        {
            chan.targetType = AnimationClip::Channel::Channel::Type::kWeights;
        }
        assert(chan.targetType != AnimationClip::Channel::Channel::Type::kNone);

        clip.channels.push_back(chan);
    }

    for (auto& s : gltfAnimation.samplers)
    {
        kame::squirtle::AnimationClip::Sampler sampler;

        auto& acc = gltf->accessors[s.input];
        auto& bv = gltf->bufferViews[acc.bufferView];
        auto& b = gltf->buffers[bv.buffer];
        for (unsigned int i = 0; i < acc.count; ++i)
        {
            auto v = ((float*)(b.decodedData.data() + bv.byteOffset + acc.byteOffset))[i];
            minTime = std::min(minTime, (double)v);
            maxTime = std::max(maxTime, (double)v);
            sampler.input.push_back(v);
        }
        sampler.minTime = sampler.input.front();
        sampler.maxTime = sampler.input.back();

        if (s.interpolation == "LINEAR")
        {
            sampler.interpolationType = AnimationClip::Sampler::InterpolationType::kLINEAR;
        }
        else if (s.interpolation == "STEP")
        {
            sampler.interpolationType = AnimationClip::Sampler::InterpolationType::kSTEP;
        }
        else if (s.interpolation == "CUBICSPLINE")
        {
            sampler.interpolationType = AnimationClip::Sampler::InterpolationType::kCUBICSPLINE;
        }

        clip.samplers.push_back(sampler);
    }

    for (auto& chan : clip.channels)
    {
        auto& sampler = clip.samplers[chan.sampler];
        auto& s = gltfAnimation.samplers[chan.sampler];

        auto& acc = gltf->accessors[s.output];
        auto& bv = gltf->bufferViews[acc.bufferView];
        auto& b = gltf->buffers[bv.buffer];
        assert(acc.componentType == GL_FLOAT);
        if (chan.targetType == AnimationClip::Channel::Type::kTranslation || chan.targetType == AnimationClip::Channel::Type::kScale)
        {
            for (unsigned int i = 0; i < acc.count; ++i)
            {
                auto v = ((kame::math::Vector3f*)(b.decodedData.data() + bv.byteOffset + acc.byteOffset))[i];
                sampler.outputVec3.push_back(v);
            }
        }
        else if (chan.targetType == AnimationClip::Channel::Type::kRotation)
        {
            for (unsigned int i = 0; i < acc.count; ++i)
            {
                auto v = ((kame::math::Vector4f*)(b.decodedData.data() + bv.byteOffset + acc.byteOffset))[i];
                sampler.outputVec4.push_back(v);
            }
        }
    }

    clip.startTime = minTime;
    clip.endTime = maxTime;

    return clip;
}

void loadGltf(kame::squirtle::Node* root, const kame::gltf::Gltf* gltf, kame::squirtle::Animation* squirtleAnimation)
{
    std::vector<kame::squirtle::Node*> squirtleNodes(gltf->nodes.size(), nullptr);

    for (auto& gltfScene : gltf->scenes)
    {
        kame::squirtle::Node* scene = new kame::squirtle::Node();
        assert(scene);

        root->addChild(scene);

        for (auto gltfNodeID : gltfScene.nodes)
        {
            loadNode(scene, gltf, gltfNodeID, squirtleNodes);
        }
    }

    squirtleAnimation->nodes = std::move(squirtleNodes);
    for (auto& gltfAnimation : gltf->animations)
    {
        squirtleAnimation->clips.emplace_back(loadAnimation(squirtleNodes, gltfAnimation, gltf));
    }
    for (auto& clip : squirtleAnimation->clips)
    {
        if (!clip.name.empty())
        {
            squirtleAnimation->clipMap[clip.name] = &clip;
        }
    }
}

kame::squirtle::GltfNode* kame::squirtle::helper::loadGltfNode(const kame::gltf::Gltf* gltf)
{
    kame::squirtle::GltfNode* node = new kame::squirtle::GltfNode();
    assert(node);

    node->animation = new kame::squirtle::Animation();
    assert(node->animation);

    loadGltf(node, gltf, node->animation);

    node->player.animation = node->animation;

    return node;
}

#include <all.hpp>

Assimp::Importer gImporter;

namespace kame::assimp {

Model* loadModelFromScene(const aiScene* scene);
Skelton loadBoneHierarchy(aiNode* rootBone);
Skelton loadSkelton(const aiScene* pScene);
void loadAnimations(Model* model, const aiScene* pScene);

Model* loadModel(const char* path)
{
    const aiScene* scene = gImporter.ReadFile(path, 0);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_ERROR("[Assimp]: {0}", gImporter.GetErrorString());
        abort();
    }

    return loadModelFromScene(scene);
}

Model* loadModelFromMemory(const void* src, size_t len, const char* hint)
{
    const aiScene* scene = gImporter.ReadFileFromMemory(src, len, 0, hint);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_ERROR("[Assimp]: {0}", gImporter.GetErrorString());
        abort();
    }

    return loadModelFromScene(scene);
}

void deleteModel(Model* model)
{
    for (auto it : model->skelton.boneMap)
    {
        delete it.second;
    }
    delete model;
    model = nullptr;
}

Model* loadModelFromScene(const aiScene* scene)
{
    assert(scene->mRootNode);

    Model* model = new Model();
    assert(model);

    loadAnimations(model, scene);
    model->skelton = loadSkelton(scene);
    model->skelton.buildRestPoseMatrices();

    std::stack<aiNode*> nodeStack;
    nodeStack.push(scene->mRootNode);
NEXT_NODE:
    aiNode* node = nodeStack.top();
    nodeStack.pop();

    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
        model->meshes.emplace_back();

        for (unsigned int vi = 0; vi < ai_mesh->mNumVertices; ++vi)
        {
            kame::math::Vector3f v;
            v.x = ai_mesh->mVertices[vi].x;
            v.y = ai_mesh->mVertices[vi].y;
            v.z = ai_mesh->mVertices[vi].z;
            model->meshes[i].positions.push_back(v);

            for (unsigned int ti = 0; ti < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++ti)
            {
                if (ai_mesh->mTextureCoords[ti])
                {
                    if (model->meshes[i].texCoords.size() < ti + 1)
                    {
                        for (unsigned int k = 0; k < (ti + 1) - model->meshes[i].texCoords.size(); ++k)
                        {
                            model->meshes[i].texCoords.emplace_back();
                        }
                    }
                    kame::math::Vector2f uv;
                    uv.x = ai_mesh->mTextureCoords[ti][vi].x;
                    uv.y = ai_mesh->mTextureCoords[ti][vi].y;
                    model->meshes[i].texCoords[ti].push_back(uv);
                }
            }

            if (ai_mesh->mNumBones)
            {
                model->meshes[i].influences.emplace_back();
            }
        }

        assert(ai_mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE);
        for (unsigned int fi = 0; fi < ai_mesh->mNumFaces; ++fi)
        {
            aiFace face = ai_mesh->mFaces[fi];
            for (unsigned int idx = 0; idx < face.mNumIndices; ++idx)
            {
                model->meshes[i].indices.push_back(face.mIndices[idx]);
            }
        }

        for (unsigned int bi = 0; bi < ai_mesh->mNumBones; ++bi)
        {
            aiBone* bone = ai_mesh->mBones[bi];
            Bone* joint = model->skelton.boneMap[bone->mName.C_Str()];
            joint->offsetMatrix = bone->mOffsetMatrix;
            for (unsigned int wi = 0; wi < bone->mNumWeights; ++wi)
            {
                aiVertexWeight vw = bone->mWeights[wi];
                BoneInfluence influence;
                influence.boneID = joint->id;
                influence.weight = vw.mWeight;
                model->meshes[i].influences[vw.mVertexId].push_back(influence);
            }
        }
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        nodeStack.push(node->mChildren[i]);
    }
    if (!nodeStack.empty())
    {
        goto NEXT_NODE;
    }

    gImporter.FreeScene();

    return model;
}

Skelton loadSkelton(const aiScene* pScene)
{
    std::unordered_set<std::string> boneNames;
    aiNode* rootBone = nullptr;

    if (pScene->HasAnimations())
    {
        aiAnimation* anim = pScene->mAnimations[0];
        for (unsigned int i = 0; i < anim->mNumChannels; ++i)
        {
            boneNames.insert(std::string(anim->mChannels[i]->mNodeName.C_Str()));
        }
    }

    std::stack<aiNode*> nodeStack;
    nodeStack.push(pScene->mRootNode);
NEXT_NODE:
    aiNode* node = nodeStack.top();
    nodeStack.pop();

    std::string name = node->mName.C_Str();
    if (boneNames.find(name) != boneNames.end())
    {
        rootBone = node;
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        nodeStack.push(node->mChildren[i]);
    }
    if (!rootBone && !nodeStack.empty())
    {
        goto NEXT_NODE;
    }

    if (rootBone)
    {
        return loadBoneHierarchy(rootBone);
    }
    return Skelton();
}

Skelton loadBoneHierarchy(aiNode* rootBone)
{
    int id = 0;
    Skelton skl;
    skl.rootBone = new Bone();
    skl.rootBone->name = rootBone->mName.C_Str();
    skl.rootBone->id = id;
    skl.boneMap[skl.rootBone->name] = skl.rootBone;

    std::stack<aiNode*> nodeStack;
    nodeStack.push(rootBone);
NEXT_NODE:
    aiNode* node = nodeStack.top();
    nodeStack.pop();

    Bone* current;
    current = skl.boneMap[node->mName.C_Str()];

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        Bone* bone = new Bone();
        bone->name = node->mChildren[i]->mName.C_Str();
        bone->parent = current;
        id += 1;
        bone->id = id;
        skl.boneMap[bone->name] = bone;

        current->children.push_back(bone);
        nodeStack.push(node->mChildren[i]);
    }
    if (!nodeStack.empty())
    {
        goto NEXT_NODE;
    }

    return skl;
}

void loadAnimations(Model* model, const aiScene* pScene)
{
    if (!pScene->HasAnimations())
    {
        return;
    }

    for (unsigned int i = 0; i < pScene->mNumAnimations; ++i)
    {
        model->animations.emplace_back();
        Animation& a = model->animations[i];
        aiAnimation* pa = pScene->mAnimations[i];
        a.name = pa->mName.C_Str();
        a.duration = pa->mDuration;
        a.ticksPerSecond = pa->mTicksPerSecond;
        if (a.ticksPerSecond < 1)
        {
            SPDLOG_WARN("aiAnimation::mTicksPerSecond < 1.0");
            a.ticksPerSecond = 24.0;
        }
        for (unsigned int j = 0; j < pa->mNumChannels; ++j)
        {
            a.channels.emplace_back();
            aiNodeAnim* na = pa->mChannels[j];
            a.channels[j].boneName = na->mNodeName.C_Str();
            a.channels[j].preState = na->mPreState;
            a.channels[j].postState = na->mPostState;
            for (unsigned int pi = 0; pi < na->mNumPositionKeys; ++pi)
            {
                a.channels[j].positions.push_back(na->mPositionKeys[pi]);
            }
            for (unsigned int ri = 0; ri < na->mNumRotationKeys; ++ri)
            {
                a.channels[j].rotations.push_back(na->mRotationKeys[ri]);
            }
            for (unsigned int si = 0; si < na->mNumScalingKeys; ++si)
            {
                a.channels[j].scales.push_back(na->mScalingKeys[si]);
            }
        }
    }
}

void Skelton::buildRestPoseMatrices()
{
    if (!rootBone)
    {
        return;
    }
    restPoseMatrices.clear();

    std::stack<Bone*> boneStack;
    boneStack.push(rootBone);
POP_STACK:
    Bone* i = boneStack.top();
    boneStack.pop();

    aiMatrix4x4 localMatrix = i->offsetMatrix;
    restPoseMatrices.push_back(localMatrix);

    aiMatrix4x4 globalMatrix;
    if (i->parent)
    {
        globalMatrix = restPoseMatrices[i->parent->id];
    }

    globalMatrix *= localMatrix;

    restPoseMatrices[i->id] = globalMatrix;

    for (Bone* c : i->children)
    {
        boneStack.push(c);
    }

    if (!boneStack.empty())
    {
        goto POP_STACK;
    }
}

} // namespace kame::assimp
#include <all.hpp>

Assimp::Importer gImporter;

namespace kame::assimp {

Model* loadModel(const char* path)
{
    const aiScene* scene = gImporter.ReadFile(path, 0);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_ERROR("[Assimp]: {0}", gImporter.GetErrorString());
        abort();
    }

    Model* model = new Model();
    assert(model);

    std::stack<aiNode*> nodeStack;
    nodeStack.push(scene->mRootNode);
    unsigned int offset = 0;
NEXT_NODE:
    aiNode* node = nodeStack.top();
    nodeStack.pop();
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];

        for (unsigned int vi = 0; vi < ai_mesh->mNumVertices; ++vi)
        {
            kame::math::Vector3f v;
            v.x = ai_mesh->mVertices[vi].x;
            v.y = ai_mesh->mVertices[vi].y;
            v.z = ai_mesh->mVertices[vi].z;
            model->mesh.positions.push_back(v);

            if (ai_mesh->mTextureCoords[0])
            {
                kame::math::Vector2f uv;
                uv.x = ai_mesh->mTextureCoords[0][vi].x;
                uv.y = ai_mesh->mTextureCoords[0][vi].y;
                model->mesh.texCoords.push_back(uv);
            }
        }

        for (unsigned int fi = 0; fi < ai_mesh->mNumFaces; ++fi)
        {
            aiFace face = ai_mesh->mFaces[fi];
            for (unsigned int idx = 0; idx < face.mNumIndices; ++idx)
            {
                model->mesh.indices.push_back(offset + face.mIndices[idx]);
            }
        }

        offset += ai_mesh->mNumVertices;
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

Model* loadModelFromMemory(const void* src, size_t len, const char* hint)
{
    const aiScene* scene = gImporter.ReadFileFromMemory(src, len, 0, hint);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_ERROR("[Assimp]: {0}", gImporter.GetErrorString());
        abort();
    }

    Model* model = new Model();
    assert(model);

    std::stack<aiNode*> nodeStack;
    nodeStack.push(scene->mRootNode);
    unsigned int offset = 0;
NEXT_NODE:
    aiNode* node = nodeStack.top();
    nodeStack.pop();
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];

        for (unsigned int vi = 0; vi < ai_mesh->mNumVertices; ++vi)
        {
            kame::math::Vector3f v;
            v.x = ai_mesh->mVertices[vi].x;
            v.y = ai_mesh->mVertices[vi].y;
            v.z = ai_mesh->mVertices[vi].z;
            model->mesh.positions.push_back(v);

            if (ai_mesh->mTextureCoords[0])
            {
                kame::math::Vector2f uv;
                uv.x = ai_mesh->mTextureCoords[0][vi].x;
                uv.y = ai_mesh->mTextureCoords[0][vi].y;
                model->mesh.texCoords.push_back(uv);
            }
        }

        for (unsigned int fi = 0; fi < ai_mesh->mNumFaces; ++fi)
        {
            aiFace face = ai_mesh->mFaces[fi];
            for (unsigned int idx = 0; idx < face.mNumIndices; ++idx)
            {
                model->mesh.indices.push_back(offset + face.mIndices[idx]);
            }
        }

        offset += ai_mesh->mNumVertices;
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

void deleteModel(Model* model)
{
    delete model;
    model = nullptr;
}

} // namespace kame::assimp
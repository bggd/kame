#include <all.hpp>

namespace kame::squirtle {

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
                for (unsigned int i = 0; i < acc.count; ++i)
                {
                    auto v = ((kame::math::Vector2f*)(b.decodedData.data() + bv.byteOffset + acc.byteOffset))[i];
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

void loadNode(kame::squirtle::Node* parent, const kame::gltf::Gltf* gltf, kame::gltf::integer gltfNodeID)
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

    parent->addChild(self);

    for (auto childID : gltfNode.children)
    {
        loadNode(self, gltf, childID);
    }
}

void loadScene(kame::squirtle::Node* root, const kame::gltf::Gltf* gltf)
{
    for (auto& gltfScene : gltf->scenes)
    {
        kame::squirtle::Node* scene = new kame::squirtle::Node();
        assert(scene);

        root->addChild(scene);

        for (auto gltfNodeID : gltfScene.nodes)
        {
            loadNode(scene, gltf, gltfNodeID);
        }
    }
}

void loadMeshNodeFromGLTF(kame::squirtle::Node* parent, const kame::gltf::Gltf* gltf)
{
    assert(parent);

    loadScene(parent, gltf);
}

} // namespace kame::squirtle

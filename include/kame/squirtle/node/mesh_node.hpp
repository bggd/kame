#pragma once

#include "node.hpp"
#include "gltf_node.hpp"

#include <kame/math/math.hpp>
#include <kame/ogl/ogl.hpp>
#include <array>
#include <vector>
#include <cstdint>

namespace kame::squirtle {

struct Mesh {
    std::vector<kame::math::Vector3f> positions;
    std::vector<kame::math::Vector2f> texCoords;
    std::vector<kame::math::Vector3f> normals;
    std::vector<std::array<uint16_t, 4>> joints;
    std::vector<kame::math::Vector4f> weights;
    std::vector<uint32_t> indices;
};

struct VBO {
    kame::ogl::VertexBuffer* vboPositions = nullptr;
    kame::ogl::VertexBuffer* vboTexCoords = nullptr;
    kame::ogl::VertexBuffer* vboNormals = nullptr;
    kame::ogl::IndexBuffer* ibo = nullptr;
    kame::ogl::VertexArrayObject vao;
    bool vaoIsCreated = false;
};

struct BufferedVBO {
    uint8_t current = 0;
    uint8_t numBuffering = 1;

    std::vector<VBO> buffers;

    void init(const Mesh* mesh)
    {
        for (int i = 0; i < numBuffering; ++i)
        {
            buffers.emplace_back();
            VBO& vbo = buffers.back();
            vbo.vboPositions = kame::ogl::createVertexBuffer(mesh->positions.size() * 3 * sizeof(float), GL_DYNAMIC_DRAW);
            vbo.vboTexCoords = kame::ogl::createVertexBuffer(mesh->texCoords.size() * 2 * sizeof(float), GL_DYNAMIC_DRAW);
            vbo.vboNormals = kame::ogl::createVertexBuffer(mesh->normals.size() * 3 * sizeof(float), GL_DYNAMIC_DRAW);
            vbo.ibo = kame::ogl::createIndexBuffer(mesh->indices.size() * sizeof(unsigned int), GL_DYNAMIC_DRAW);
        }
    }

    void swap()
    {
        current++;
        if (current >= numBuffering)
        {
            current = 0;
        }
    }

    VBO& getVBO()
    {
        return buffers[current];
    }

    void updateVBO(const Mesh* mesh)
    {
        VBO& vbo = getVBO();
        vbo.vboPositions->setBuffer((const float*)&mesh->positions[0]);
        vbo.vboTexCoords->setBuffer((const float*)&mesh->texCoords[0]);
        vbo.vboNormals->setBuffer((const float*)&mesh->normals[0]);
        vbo.ibo->setBuffer((const unsigned int*)&mesh->indices[0]);
    }
};

struct MeshNode : kame::squirtle::Node {

    Mesh* mesh = nullptr;
    kame::ogl::Texture2D* diffuse = nullptr;
    BufferedVBO bufferedVBO;
    uint32_t skinIdx = 0;
    GltfNode* gltf = nullptr;

    SquirtleNodeType getType() const override
    {
        return kSquirtleMeshNode;
    }
};

} // namespace kame::squirtle

#include <kame/kame.hpp>
#include <vector>
#include <cstdio>

#include <pystring.h>

int getNumberOfUVSet(const kame::gltf::Gltf* gltf)
{
    int num = 0;

    for (auto& m : gltf->meshes)
    {
        for (auto& pri : m.primitives)
        {
            for (auto& item : pri.attributes)
            {
                if (pystring::startswith(item.first, "TEXCOORD"))
                {
                    ++num;
                }
            }
        }
    }

    return num;
}

struct Mesh {
    std::vector<kame::math::Vector3> positions;
    std::vector<std::vector<kame::math::Vector2>> uvSets;
    std::vector<unsigned int> indices;
};

struct VBOMesh {
    kame::ogl::VertexBuffer* vboPositions = nullptr;
    std::vector<kame::ogl::VertexBuffer*> vboUVSets;
    kame::ogl::IndexBuffer* iboIndices = nullptr;

    VBOMesh(const Mesh& mesh)
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
    }

    ~VBOMesh()
    {
        kame::ogl::deleteIndexBuffer(iboIndices);
        for (auto& uv : vboUVSets)
        {
            kame::ogl::deleteVertexBuffer(uv);
        }
        kame::ogl::deleteVertexBuffer(vboPositions);
    }
};

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
                    auto v = ((kame::math::Vector3*)(b.decodedData.data() + bv.byteOffset + acc.byteOffset))[i];
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

    int numUV = getNumberOfUVSet(gltf);
    uvSets.resize(numUV);

    for (auto& pri : m.primitives)
    {
        for (auto& item : pri.attributes)
        {
            if (pystring::startswith(item.first, "TEXCOORD"))
            {
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
                        auto v = ((kame::math::Vector2*)(b.decodedData.data() + bv.byteOffset + acc.byteOffset))[i];
                        uvSets[uvid].push_back(v);
                    }
                    else if (acc.componentType == GL_UNSIGNED_BYTE)
                    {
                        auto e = ((unsigned char*)(b.decodedData.data() + bv.byteOffset + acc.byteOffset))[i];
                        uvSets[uvid].push_back(e / 255.0f);
                    }
                    else if (acc.componentType == GL_UNSIGNED_SHORT)
                    {
                        auto e = ((unsigned short*)(b.decodedData.data() + bv.byteOffset + acc.byteOffset))[i];
                        uvSets[uvid].push_back(e / 65535.0f);
                    }
                }
            }
        }
    }

    return uvSets;
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
    }

    return indices;
}

const char* vertGLSL = R"(#version 330
in vec2 vUV;
uniform mat4 uMVP;
void main() {
    gl_Position = uMVP * vec4(vUV.x, vUV.y, 0.0, 1.0);
}
)";

const char* fragGLSL = R"(#version 330
out vec4 fragColor;
void main() {
    fragColor = vec4(0.278431, 0.447059, 0.701961, 0.25);
}
)";

const char* flippedUVGLSL = R"(#version 330
out vec4 fragColor;
void main() {
    if (gl_FrontFacing) {
        discard;
    }
    else {
        fragColor = vec4(0.701961, 0.278431, 0.278431, 0.25);
    }
}
)";

const char* drawLinesGLSL = R"(#version 330
out vec4 fragColor;
void main() {
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
)";

const char* drawPointsGLSL = R"(#version 330 core
out vec4 fragColor;
void main() {
    fragColor = vec4(1.0);
}
)";

using namespace kame::math;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "require *.gltf\n");
        return 1;
    }

    kame::kameInit();

    kame::sdl::WindowOGL win;
    win.setOglDebugMode(true);
    win.forceGLVersion(3, 3, false);
    win.openWindow("uvview", 512, 512);
    win.setVsync(true);

    kame::ogl::Shader* shaderFrontFace = kame::ogl::createShader(vertGLSL, fragGLSL);
    kame::ogl::Shader* shaderBackFace = kame::ogl::createShader(vertGLSL, flippedUVGLSL);
    kame::ogl::Shader* shaderDrawLines = kame::ogl::createShader(vertGLSL, drawLinesGLSL);
    kame::ogl::Shader* shaderDrawPoints = kame::ogl::createShader(vertGLSL, drawPointsGLSL);

    kame::gltf::Gltf* gltf = kame::gltf::loadGLTF(argv[1]);
    int numUVSet = getNumberOfUVSet(gltf);
    fprintf(stdout, "Number Of UV Set: %d\n", numUVSet);
    if (numUVSet < 1)
    {
        fprintf(stderr, "require TEXCOORD_* in gltf\n");
        return 1;
    }
    std::vector<Mesh> meshes;
    std::vector<VBOMesh> vboMeshes;
    meshes.reserve(gltf->meshes.size());
    vboMeshes.reserve(gltf->meshes.size());
    for (auto& m : gltf->meshes)
    {
        meshes.emplace_back();
        auto& mesh = meshes.back();

        mesh.positions = toVertexPositions(gltf, m);
        mesh.uvSets = toVertexUVSets(gltf, m);
        mesh.indices = toVertexIndices(gltf, m);

        vboMeshes.emplace_back(mesh);
    }
    kame::gltf::deleteGLTF(gltf);

    std::vector<kame::ogl::VertexArrayObject> vaoList;
    for (auto& vbo : vboMeshes)
    {
        auto vao = kame::ogl::VertexArrayObjectBuilder()
                       .bindAttribute(shaderFrontFace->getAttribLocation("vUV"), vbo.vboUVSets[0], 2, 2 * sizeof(float), 0)
                       .bindIndexBuffer(vbo.iboIndices)
                       .build();
        vaoList.push_back(vao);
    }

    for (;;)
    {
        win.updateInput();
        auto state = win.getState();
        if (state.isCloseRequest)
            break;

        kame::ogl::setViewport(0, 0, state.drawableSizeX, state.drawableSizeY);
        kame::ogl::setClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, Vector4(0.5, 0.5, 0.5, 1));
        kame::ogl::RasterizerState rasterState = kame::ogl::RasterizerStateBuilder().build();
        kame::ogl::setRasterizerState(rasterState);
        kame::ogl::BlendState blendState = kame::ogl::BlendStateBuilder()
                                               .blendEquation(GL_FUNC_ADD, GL_FUNC_ADD)
                                               .blendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA)
                                               .build();
        kame::ogl::setBlendState(blendState);

        auto view = Matrix::createLookAt(Vector3(0.0f, 0.0f, 0.5f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        auto proj = Matrix::createOrthographic_NO(0.0f, 1.0f, 1.0f, 0.0f, 0.1f, 10.0f);

        kame::ogl::setShader(shaderFrontFace);
        shaderFrontFace->setMatrix("uMVP", view * proj);
        int idx = 0;
        for (auto& m : meshes)
        {
            vaoList[idx].drawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT);
            ++idx;
        }

        kame::ogl::setShader(shaderBackFace);
        shaderBackFace->setMatrix("uMVP", view * proj);
        idx = 0;
        for (auto& m : meshes)
        {
            vaoList[idx].drawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT);
            ++idx;
        }

        kame::ogl::setShader(shaderDrawLines);
        shaderDrawLines->setMatrix("uMVP", view * proj);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        idx = 0;
        for (auto& m : meshes)
        {
            vaoList[idx].drawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT);
            ++idx;
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        kame::ogl::setShader(shaderDrawPoints);
        shaderDrawPoints->setMatrix("uMVP", view * proj);
        idx = 0;
        for (auto& m : meshes)
        {
            vaoList[idx].drawElements(GL_POINTS, m.indices.size(), GL_UNSIGNED_INT);
            ++idx;
        }

        win.swapWindow();
    }

    vaoList.clear();
    vboMeshes.clear();
    kame::ogl::deleteShader(shaderDrawPoints);
    kame::ogl::deleteShader(shaderDrawLines);
    kame::ogl::deleteShader(shaderBackFace);
    kame::ogl::deleteShader(shaderFrontFace);
    win.closeWindow();
    kame::kameShutdown();
    return 0;
}

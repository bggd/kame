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
    win.forceGLVersion(3, 3, true);
    win.openWindow();
    win.setVsync(true);

    kame::gltf::Gltf* gltf = kame::gltf::loadGLTF(argv[1]);
    int numUVSet = getNumberOfUVSet(gltf);
    fprintf(stdout, "Number Of UV Set: %d\n", numUVSet);
    if (numUVSet < 1)
    {
        fprintf(stderr, "require TEXCOORD_* in gltf\n");
        return 1;
    }
    std::vector<Mesh> meshes;
    for (auto& m : gltf->meshes)
    {
        meshes.emplace_back();
        auto& mesh = meshes.back();

        mesh.positions = toVertexPositions(gltf, m);
        mesh.uvSets = toVertexUVSets(gltf, m);
        mesh.indices = toVertexIndices(gltf, m);
    }

    for (;;)
    {
        win.updateInput();
        auto state = win.getState();
        if (state.isCloseRequest)
            break;
        win.swapWindow();
    }

    kame::gltf::deleteGLTF(gltf);
    win.closeWindow();
    kame::kameShutdown();
    return 0;
}

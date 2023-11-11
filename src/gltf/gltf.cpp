#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <all.hpp>

namespace kame::gltf {

std::vector<uint8_t> decodeBase64(const std::string& str, integer start);

extern void loadTextures(Gltf* gltf, json& j);
extern void loadImages(Gltf* gltf, json& j);
extern void loadSamplers(Gltf* gltf, json& j);
extern void loadMaterials(Gltf* gltf, json& j);

void loadScenes(Gltf* gltf, json& j)
{
    if (j.contains("scenes"))
    {
        for (auto& e : j["scenes"])
        {
            Scene scene;
            if (e.contains("name"))
            {
                scene.name = e["name"].get<std::string>();
            }
            if (e.contains("nodes"))
            {
                for (auto& n : e["nodes"])
                {
                    scene.nodes.push_back(n.get<integer>());
                }
            }
            gltf->scenes.push_back(scene);
        }
    }
}

void loadNodes(Gltf* gltf, json& j)
{
    if (j.contains("nodes"))
    {
        for (auto& e : j["nodes"])
        {
            Node node;
            if (e.contains("name"))
            {
                node.name = e["name"].get<std::string>();
            }
            if (e.contains("children"))
            {
                for (auto& n : e["children"])
                {
                    node.children.push_back(n.get<integer>());
                }
            }
            if (e.contains("skin"))
            {
                node.skin = e["skin"].get<integer>();
                node.hasSkin = true;
            }
            if (e.contains("mesh"))
            {
                node.mesh = e["mesh"].get<integer>();
                node.hasMesh = true;
            }
            if (e.contains("matrix"))
            {
                assert(e["matrix"].size() == 16);
                int i = 0;
                for (auto& n : e["matrix"])
                {
                    node.matrix[i] = n.get<float>();
                    ++i;
                }
                node.hasMatrix = true;
            }
            if (e.contains("translation"))
            {
                assert(e["translation"].size() == 3);
                int i = 0;
                for (auto& n : e["translation"])
                {
                    node.translation[i] = n.get<float>();
                    ++i;
                }
                node.hasTranslation = true;
            }
            if (e.contains("rotation"))
            {
                assert(e["rotation"].size() == 4);
                int i = 0;
                for (auto& n : e["rotation"])
                {
                    node.rotation[i] = n.get<float>();
                    ++i;
                }
                node.hasRotation = true;
            }
            if (e.contains("scale"))
            {
                assert(e["scale"].size() == 3);
                int i = 0;
                for (auto& n : e["scale"])
                {
                    node.scale[i] = n.get<float>();
                    ++i;
                }
                node.hasScale = true;
            }
            gltf->nodes.push_back(node);
        }
    }
}

void loadBuffers(Gltf* gltf, json& j)
{
    if (j.contains("buffers"))
    {
        for (auto& e : j["buffers"])
        {
            Buffer buffer;
            buffer.byteLength = e["byteLength"].get<integer>();
            if (e.contains("name"))
            {
                buffer.name = e["name"].get<std::string>();
            }
            if (e.contains("uri"))
            {
                buffer.uri = e["uri"].get<std::string>();
                if (pystring::startswith(buffer.uri, "data:application/octet-stream;base64,"))
                {
                    assert(((buffer.uri.size() - std::string("data:application/octet-stream;base64,").size()) % 4) == 0);
                    buffer.binaryData = decodeBase64(buffer.uri, std::string("data:application/octet-stream;base64,").size());
                    assert(buffer.binaryData.size() == buffer.byteLength);
                }
                else if (pystring::endswith(buffer.uri, ".bin"))
                {
                    std::filesystem::path path(gltf->basePath);
                    path /= buffer.uri;
                    SDL_RWops* fp = SDL_RWFromFile(path.string().c_str(), "rb");
                    assert(fp);
                    Sint64 len = SDL_RWsize(fp);
                    assert(len >= 0);
                    assert(kame::gltf::integer(len) == buffer.byteLength);
                    buffer.binaryData.resize(len + 1);

                    Sint64 nb_read_total = 0, nb_read = 1;
                    auto* buf = buffer.binaryData.data();
                    while (nb_read_total < len && nb_read != 0)
                    {
                        nb_read = SDL_RWread(fp, buf, (len - nb_read_total));
                        nb_read_total += nb_read;
                        buf += nb_read;
                    }
                    SDL_RWclose(fp);
                    assert(nb_read_total == len);

                    buffer.binaryData.back() = '\0';
                }
            }
            gltf->buffers.push_back(buffer);
        }
    }
}

void loadBufferViews(Gltf* gltf, json& j)
{
    if (j.contains("bufferViews"))
    {
        for (auto& e : j["bufferViews"])
        {
            BufferView view;
            if (e.contains("name"))
            {
                view.name = e["name"].get<std::string>();
            }
            if (e.contains("byteOffset"))
            {
                view.byteOffset = e["byteOffset"].get<integer>();
                view.hasByteOffset = true;
            }
            if (e.contains("byteStride"))
            {
                view.byteStride = e["byteStride"].get<integer>();
                view.hasByteStride = true;
            }
            if (e.contains("target"))
            {
                view.target = e["target"].get<integer>();
                view.hasTarget = true;
            }
            view.buffer = e["buffer"].get<integer>();
            view.byteLength = e["byteLength"].get<integer>();
            gltf->bufferViews.push_back(view);
        }
    }
}

void loadAccessors(Gltf* gltf, json& j)
{
    if (j.contains("accessors"))
    {
        for (auto& e : j["accessors"])
        {
            Accessor accessor;
            if (e.contains("name"))
            {
                accessor.name = e["name"].get<std::string>();
            }
            if (e.contains("bufferView"))
            {
                accessor.bufferView = e["bufferView"].get<integer>();
                accessor.hasBufferView = true;
            }
            if (e.contains("byteOffset"))
            {
                accessor.byteOffset = e["byteOffset"].get<integer>();
                accessor.hasByteOffset = true;
            }
            if (e.contains("normalized"))
            {
                accessor.normalized = e["normalized"].get<bool>();
                accessor.hasNormalized = true;
            }
            if (e.contains("max"))
            {
                for (auto& x : e["max"])
                {
                    accessor.max.push_back(x.get<integer>());
                }
            }
            if (e.contains("min"))
            {
                for (auto& x : e["min"])
                {
                    accessor.min.push_back(x.get<integer>());
                }
            }
            accessor.componentType = e["componentType"].get<integer>();
            accessor.count = e["count"].get<integer>();
            accessor.type = e["type"].get<std::string>();

            gltf->accessors.push_back(accessor);
        }
    }
}

void loadMeshes(Gltf* gltf, json& j)
{
    if (j.contains("meshes"))
    {
        for (auto& e : j["meshes"])
        {
            Mesh mesh;
            if (e.contains("name"))
            {
                mesh.name = e["name"].get<std::string>();
            }
            for (auto& p : e["primitives"])
            {
                Mesh::Primitive primitive;
                if (p.contains("indices"))
                {
                    primitive.indices = p["indices"].get<integer>();
                    primitive.hasIndices = true;
                }
                if (p.contains("mode"))
                {
                    primitive.mode = p["mode"].get<integer>();
                    primitive.hasMode = true;
                }
                for (auto& [key, val] : p["attributes"].items())
                {
                    primitive.attributes.push_back(std::make_pair(key, val));
                }
                mesh.primitives.push_back(primitive);
            }
            gltf->meshes.push_back(mesh);
        }
    }
}

void loadAnimations(Gltf* gltf, json& j)
{
    if (j.contains("animations"))
    {
        for (auto& e : j["animations"])
        {
            Animation anim;
            if (e.contains("name"))
            {
                anim.name = e["name"].get<std::string>();
            }
            for (auto& c : e["channels"])
            {
                Animation::Channel animChan;
                animChan.sampler = c["sampler"].get<integer>();
                if (c["target"].contains("node"))
                {
                    animChan.target.node = c["target"]["node"].get<integer>();
                    animChan.target.hasNode = true;
                }
                animChan.target.path = c["target"]["path"].get<std::string>();
                anim.channels.push_back(animChan);
            }
            for (auto& s : e["samplers"])
            {
                Animation::Sampler animSampler;
                animSampler.input = s["input"].get<integer>();
                if (s.contains("interpolation"))
                {
                    animSampler.interpolation = s["interpolation"].get<std::string>();
                }
                animSampler.output = s["output"].get<integer>();
                anim.samplers.push_back(animSampler);
            }
            gltf->animations.push_back(anim);
        }
    }
}

void loadSkins(Gltf* gltf, json& j)
{
    if (j.contains("skins"))
    {
        for (auto& e : j["skins"])
        {
            Skin skin;
            if (e.contains("name"))
            {
                skin.name = e["name"].get<std::string>();
            }
            if (e.contains("inverseBindMatrices"))
            {
                skin.inverseBindMatrices = e["inverseBindMatrices"].get<integer>();
                skin.hasInverseBindMatrices = true;
            }
            if (e.contains("skeleton"))
            {
                skin.skeleton = e["skeleton"].get<integer>();
                skin.hasSkeleton = true;
            }
            for (auto& i : e["joints"])
            {
                skin.joints.push_back(i.get<integer>());
            }
            gltf->skins.push_back(skin);
        }
    }
}

Gltf* loadGLTF(const char* path)
{
    Gltf* gltf = new Gltf();
    assert(gltf);

    SPDLOG_DEBUG("[Gltf] loading: {0}", path);
    std::fstream f(path);
    json j = json::parse(f, nullptr);
    assert(!j.is_discarded());
    gltf->path = std::string(path);
    gltf->basePath = std::filesystem::path(gltf->path).parent_path().string();

    if (j.contains("scene"))
    {
        gltf->scene = j["scene"].get<integer>();
        gltf->hasScene = true;
    }

    loadScenes(gltf, j);
    loadNodes(gltf, j);
    loadBuffers(gltf, j);
    loadBufferViews(gltf, j);
    loadAccessors(gltf, j);
    loadMeshes(gltf, j);
    loadAnimations(gltf, j);
    loadSkins(gltf, j);
    loadTextures(gltf, j);
    loadImages(gltf, j);
    loadSamplers(gltf, j);
    loadMaterials(gltf, j);

    SPDLOG_DEBUG("[Gltf] loaded: {0}", path);

    return gltf;
}

Gltf* loadGLTFFromMemory(const unsigned char* src, unsigned int len)
{
    Gltf* gltf = new Gltf();
    assert(gltf);

    json j = json::parse(src, src + len, nullptr);
    assert(!j.is_discarded());

    if (j.contains("scene"))
    {
        gltf->scene = j["scene"].get<integer>();
        gltf->hasScene = true;
    }

    loadScenes(gltf, j);
    loadNodes(gltf, j);
    loadBuffers(gltf, j);
    loadBufferViews(gltf, j);
    loadAccessors(gltf, j);
    loadMeshes(gltf, j);
    loadAnimations(gltf, j);
    loadSkins(gltf, j);
    loadTextures(gltf, j);
    loadImages(gltf, j);
    loadSamplers(gltf, j);
    loadMaterials(gltf, j);

    return gltf;
}

void deleteGLTF(Gltf* gltf)
{
    delete gltf;
}

int b64ToInt(std::string c)
{
    if (pystring::isalpha(c))
    {
        if (pystring::isupper(c))
        {
            return c[0] - 'A';
        }
        else
        {
            return c[0] - 'a' + 26;
        }
    }
    else if (pystring::isdigit(c))
    {
        return c[0] - '0' + 52;
    }
    else if (c == "+")
    {
        return 62;
    }
    else if (c == "/")
    {
        return 63;
    }
    else if (c == "=")
    {
        return 64;
    }
    else
    {
        return -1;
    }
}

std::vector<uint8_t> decodeBase64(const std::string& str, integer start)
{
    std::vector<uint8_t> data;

    for (integer i = start; i < str.size(); i += 4)
    {
        int decode[4];
        unsigned int bytes;

        decode[0] = b64ToInt(std::string{str[i + 0]});
        decode[1] = b64ToInt(std::string{str[i + 1]});
        decode[2] = b64ToInt(std::string{str[i + 2]});
        decode[3] = b64ToInt(std::string{str[i + 3]});
        for (int n : decode)
        {
            assert(n >= 0);
        }

        if (decode[2] == 64 && decode[3] == 64)
        {
            decode[1] = decode[1] & 0b11110000;
            bytes = decode[0] << 6 | decode[1];
            bytes = bytes << 12;
            data.push_back(bytes >> 16);
        }
        else if (decode[3] == 64)
        {
            decode[2] = decode[2] & 0b11111100;
            bytes = decode[0] << 6 | decode[1];
            bytes = bytes << 6 | decode[2];
            bytes = bytes << 6;
            data.push_back(bytes >> 16);
            data.push_back(bytes >> 8);
        }
        else
        {
            bytes = decode[0] << 6 | decode[1];
            bytes = bytes << 6 | decode[2];
            bytes = bytes << 6 | decode[3];
            data.push_back(bytes >> 16);
            data.push_back(bytes >> 8);
            data.push_back(bytes);
        }
    }

    return data;
}

} // namespace kame::gltf

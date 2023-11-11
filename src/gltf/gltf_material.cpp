#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <all.hpp>

namespace kame::gltf {

void loadTextures(Gltf* gltf, json& j)
{
    if (!j.contains("textures"))
    {
        return;
    }

    for (auto& e : j["textures"])
    {
        Texture texture;
        if (e.contains("sampler"))
        {
            texture.sampler = e["sampler"].get<integer>();
            texture.hasSampler = true;
        }
        if (e.contains("source"))
        {
            texture.source = e["source"].get<integer>();
            texture.hasSource = true;
        }
        gltf->textures.push_back(texture);
    }
}

void loadImages(Gltf* gltf, json& j)
{
    if (!j.contains("images"))
    {
        return;
    }

    for (auto& e : j["images"])
    {
        Image image;
        if (e.contains("uri"))
        {
            image.uri = e["uri"].get<std::string>();
            image.hasURI = true;
        }
        if (e.contains("mimeType"))
        {
            image.mimeType = e["mimeType"].get<std::string>();
            image.hasMimeType = true;
        }
        if (e.contains("bufferView"))
        {
            image.bufferView = e["bufferView"].get<integer>();
            image.hasBufferView = true;
        }
        gltf->images.push_back(image);
    }
}

void loadSamplers(Gltf* gltf, json& j)
{
    if (!j.contains("sampler"))
    {
        return;
    }

    for (auto& e : j["sampler"])
    {
        Sampler sampler;
        if (e.contains("magFilter"))
        {
            sampler.magFilter = e["magFilter"].get<integer>();
            sampler.hasMagFilter = true;
        }
        if (e.contains("minFilter"))
        {
            sampler.minFilter = e["minFilter"].get<integer>();
            sampler.hasMinFilter = true;
        }
        if (e.contains("wrapS"))
        {
            sampler.wrapS = e["wrapS"].get<integer>();
        }
        if (e.contains("wrapT"))
        {
            sampler.wrapT = e["wrapT"].get<integer>();
        }
        gltf->samplers.push_back(sampler);
    }
}

void loadMaterials(Gltf* gltf, json& j)
{
    if (!j.contains("materials"))
    {
        return;
    }

    for (auto& e : j["materials"])
    {
        Material material;
        if (e.contains("pbrMetallicRoughness"))
        {
            auto& pbr = e["pbrMetallicRoughness"];
            if (pbr.contains("baseColor"))
            {
                int i = 0;
                for (auto& n : pbr["baseColor"])
                {
                    material.pbrMetallicRoughness.baseColor[i] = n.get<float>();
                    ++i;
                }
            }
            if (pbr.contains("textureInfo"))
            {
                auto& ti = pbr["textureInfo"];
                assert(ti.contains("index"));
                material.pbrMetallicRoughness.textureInfo.index = ti["index"].get<integer>();

                if (ti.contains("texCoord"))
                {
                    material.pbrMetallicRoughness.textureInfo.texCoord = ti["texCoord"].get<integer>();
                }
            }

            if (pbr.contains("metallicFactor"))
            {
                material.pbrMetallicRoughness.metallicFactor = pbr["metallicFactor"].get<float>();
            }

            if (pbr.contains("roughnessFactor"))
            {
                material.pbrMetallicRoughness.roughnessFactor = pbr["roughnessFactor"].get<float>();
            }

            if (pbr.contains("metallicRoughnessTexture"))
            {
                auto& ti = pbr["metallicRoughnessTexture"];
                assert(ti.contains("index"));
                material.pbrMetallicRoughness.metallicRoughnessTexture.index = ti["index"].get<integer>();

                if (ti.contains("texCoord"))
                {
                    material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord = ti["texCoord"].get<integer>();
                    material.pbrMetallicRoughness.hasMetallicRoughnessTexture = true;
                }
            }
        }
        if (e.contains("doubleSided"))
        {
            material.doubleSided = e["doubleSided"].get<bool>();
        }
        gltf->materials.push_back(material);
    }
}

} // namespace kame::gltf

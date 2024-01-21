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
        gltf->textures.emplace_back(texture);
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
        gltf->images.emplace_back(image);
    }
}

void loadSamplers(Gltf* gltf, json& j)
{
    if (!j.contains("samplers"))
    {
        return;
    }

    for (auto& e : j["samplers"])
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
        gltf->samplers.emplace_back(sampler);
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
            if (pbr.contains("baseColorFactor"))
            {
                int i = 0;
                for (auto& n : pbr["baseColorFactor"])
                {
                    material.pbrMetallicRoughness.baseColorFactor[i] = n.get<float>();
                    ++i;
                }
            }
            if (pbr.contains("baseColorTexture"))
            {
                auto& ti = pbr["baseColorTexture"];
                assert(ti.contains("index"));
                material.pbrMetallicRoughness.baseColorTexture.index = ti["index"].get<integer>();

                if (ti.contains("texCoord"))
                {
                    material.pbrMetallicRoughness.baseColorTexture.texCoord = ti["texCoord"].get<integer>();
                }

                material.pbrMetallicRoughness.hasBaseColorTexture = true;
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
        if (e.contains("normalTexture"))
        {
            auto& normal = e["normalTexture"];

            material.normalTexture.index = normal["index"].get<integer>();
            if (normal["texCoord"])
            {
                material.normalTexture.texCoord = normal["texCoord"].get<integer>();
            }

            if (normal["scale"])
            {
                material.normalTexture.scale = normal["scale"].get<float>();
            }

            material.hasNormalTexture = true;
        }
        if (e.contains("alphaMode"))
        {
            material.alphaMode = e["alphaMode"].get<std::string>();
        }
        if (e.contains("alphaCutoff"))
        {
            material.alphaCutoff = e["alphaCutoff"].get<float>();
        }
        if (e.contains("doubleSided"))
        {
            material.doubleSided = e["doubleSided"].get<bool>();
        }
        gltf->materials.emplace_back(material);
    }
}

} // namespace kame::gltf

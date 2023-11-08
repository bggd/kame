#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <all.hpp>

namespace kame::gltf {

void loadTextures(Gltf* gltf, json& j)
{
    if (j.contains("textures"))
    {
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
}

void loadImages(Gltf* gltf, json& j)
{
    if (j.contains("images"))
    {
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
}

void loadSamplers(Gltf* gltf, json& j)
{
    if (j.contains("sampler"))
    {
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
}

} // namespace kame::gltf

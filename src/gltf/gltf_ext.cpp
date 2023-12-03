#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <all.hpp>

namespace kame::gltf {

void loadExtensionsForNode(Node& node, json& j)
{
    if (j.contains("EXT_mesh_gpu_instancing"))
    {
        if (node.extensions == nullptr)
        {
            node.extensions = new Ext();
            node.hasExtensions = true;
        }
        ExtMeshGPUInstancing* ext = new ExtMeshGPUInstancing();
        for (auto& [key, val] : j["EXT_mesh_gpu_instancing"]["attributes"].items())
        {
            ext->attributes.emplace_back(std::make_pair(key, val));
        }
        node.extensions->EXT_mesh_gpu_instancing = ext;
        node.extensions->hasEXT_mesh_gpu_instancing = true;
    }
}

} // namespace kame::gltf

#include <all.hpp>

namespace kame::squirtle {

void importMaterial(Model* model, const kame::gltf::Gltf* gltf)
{
    model->materials.reserve(gltf->materials.size());
    for (auto& m : gltf->materials)
    {
        Material mat;
        if (m.alphaMode == "MASK")
        {
            mat.alphaMode = kALPHA_MODE_MASK;
        }
        else if (m.alphaMode == "BLEND")
        {
            mat.alphaMode = kALPHA_MODE_BLEND;
        }
        mat.alphaCutoff = m.alphaCutoff;
        mat.doubleSided = m.doubleSided;
        mat.baseColorFactor.x = m.pbrMetallicRoughness.baseColorFactor[0];
        mat.baseColorFactor.y = m.pbrMetallicRoughness.baseColorFactor[1];
        mat.baseColorFactor.z = m.pbrMetallicRoughness.baseColorFactor[2];
        mat.baseColorFactor.w = m.pbrMetallicRoughness.baseColorFactor[3];
        if (m.pbrMetallicRoughness.hasBaseColorTexture)
        {
            mat.baseColorTextureIndex = m.pbrMetallicRoughness.baseColorTexture.index;
            mat.baseColorTexCoord = m.pbrMetallicRoughness.baseColorTexture.texCoord;
        }
        model->materials.emplace_back(mat);
    }
}

} // namespace kame::squirtle

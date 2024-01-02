#include <all.hpp>

namespace kame::squirtle {

void importMaterial(Model* model, const kame::gltf::Gltf* gltf)
{
    model->images.reserve(gltf->images.size());
    for (auto& i : gltf->images)
    {
        Image img;
        img.mimeType = i.mimeType;
        img.url = i.uri;
        model->images.emplace_back(img);
    }

    model->textures.reserve(gltf->textures.size());
    for (auto& t : gltf->textures)
    {
        assert(t.hasSource);

        Texture tex;
        tex.imageIndex = t.source;
        if (t.hasSampler)
        {
            auto sampler = gltf->samplers[t.sampler];

            if (sampler.hasMagFilter)
            {
                assert(sampler.magFilter == GL_NEAREST || sampler.magFilter == GL_LINEAR);
                tex.magFilter = sampler.magFilter;
            }

            if (sampler.hasMinFilter)
            {
                assert(sampler.minFilter == GL_NEAREST || sampler.minFilter == GL_LINEAR || sampler.minFilter == GL_NEAREST_MIPMAP_NEAREST || sampler.minFilter == GL_LINEAR_MIPMAP_NEAREST || sampler.minFilter == GL_NEAREST_MIPMAP_LINEAR || sampler.minFilter == GL_LINEAR_MIPMAP_LINEAR);
                tex.minFilter = sampler.minFilter;
            }

            assert(sampler.wrapS == GL_CLAMP_TO_EDGE || sampler.wrapS == GL_MIRRORED_REPEAT || sampler.wrapS == GL_REPEAT);
            tex.wrapS = sampler.wrapS;
            assert(sampler.wrapT == GL_CLAMP_TO_EDGE || sampler.wrapT == GL_MIRRORED_REPEAT || sampler.wrapT == GL_REPEAT);
            tex.wrapT = sampler.wrapT;
        }
        model->textures.emplace_back(tex);
    }

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

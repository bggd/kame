#include <all.hpp>

namespace kame::squirtle {

void importMaterial(Model* model, const kame::gltf::Gltf* gltf)
{
    model->textures.reserve(gltf->textures.size());
    for (auto& t : gltf->textures)
    {
        assert(t.hasSource);
        Texture tex;
        auto& img = gltf->images[t.source];
        if (img.hasURI)
        {
            std::filesystem::path path(gltf->basePath);
            path /= img.uri;
            tex.gpuTex = kame::ogl::loadTexture2D(path.string().c_str());
        }
        else
        {
            assert(img.hasBufferView);
            auto& bv = gltf->bufferViews[img.bufferView];
            auto& b = gltf->buffers[bv.buffer];
            tex.gpuTex = kame::ogl::loadTexture2DFromMemory(b.data(), b.byteLength);
        }

        if (t.hasSampler)
        {
            auto& sampler = gltf->samplers[t.sampler];
            if (sampler.hasMagFilter)
            {
                assert(sampler.magFilter == GL_NEAREST || sampler.magFilter == GL_LINEAR);
                tex.gpuTex->setTexParameteri(GL_TEXTURE_MAG_FILTER, sampler.magFilter);
            }
            if (sampler.hasMinFilter)
            {
                assert(sampler.minFilter == GL_NEAREST || sampler.minFilter == GL_LINEAR || sampler.minFilter == GL_NEAREST_MIPMAP_NEAREST || sampler.minFilter == GL_LINEAR_MIPMAP_NEAREST || sampler.minFilter == GL_NEAREST_MIPMAP_LINEAR || sampler.minFilter == GL_LINEAR_MIPMAP_LINEAR);
                tex.gpuTex->setTexParameteri(GL_TEXTURE_MIN_FILTER, sampler.minFilter);
                if (sampler.minFilter == GL_NEAREST_MIPMAP_NEAREST || sampler.minFilter == GL_LINEAR_MIPMAP_NEAREST || sampler.minFilter == GL_NEAREST_MIPMAP_LINEAR || sampler.minFilter == GL_LINEAR_MIPMAP_LINEAR)
                {
                    tex.gpuTex->generateMipmap();
                }
            }
            assert(sampler.wrapS == GL_CLAMP_TO_EDGE || sampler.wrapS == GL_MIRRORED_REPEAT || sampler.wrapS == GL_REPEAT);
            tex.gpuTex->setTexParameteri(GL_TEXTURE_WRAP_S, sampler.wrapS);
            assert(sampler.wrapT == GL_CLAMP_TO_EDGE || sampler.wrapT == GL_MIRRORED_REPEAT || sampler.wrapT == GL_REPEAT);
            tex.gpuTex->setTexParameteri(GL_TEXTURE_WRAP_T, sampler.wrapT);
        }

        model->textures.emplace_back(tex);
    }

    model->materials.reserve(gltf->materials.size());
    for (auto& m : gltf->materials)
    {
        Material mat;
        mat.doubleSided = m.doubleSided;
        mat.baseColorFactor.x = m.pbrMetallicRoughness.baseColorFactor[0];
        mat.baseColorFactor.y = m.pbrMetallicRoughness.baseColorFactor[1];
        mat.baseColorFactor.z = m.pbrMetallicRoughness.baseColorFactor[2];
        mat.baseColorFactor.w = m.pbrMetallicRoughness.baseColorFactor[3];
        if (m.pbrMetallicRoughness.hasBaseColorTexture)
        {
            mat.baseColorTexture = m.pbrMetallicRoughness.baseColorTexture.index;
            mat.baseColorTexCoord = m.pbrMetallicRoughness.baseColorTexture.texCoord;
        }
        model->materials.emplace_back(mat);
    }
}

} // namespace kame::squirtle

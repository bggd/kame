#pragma once

#include "asset_manager.hpp"

namespace kame::squirtle {

struct EngineOGL {
    AssetManager<kame::ogl::Texture2D> assetManagerTexture2D;
    AssetManager<kame::gltf::Gltf> assetManagerGltf;

    void startup();
    void shutdown();
};

} // namespace kame::squirtle

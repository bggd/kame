#pragma once

#include <kame/ogl21/ogl21.hpp>
#include <kame/squirtle/node/node.hpp>
#include <kame/squirtle/node/camera_node.hpp>
#include <kame/squirtle/node/mesh_node.hpp>
#include <kame/squirtle/node/light_node.hpp>

#include <vector>
#include <cstdint>

namespace kame::squirtle {

struct Engine {
    kame::ogl21::Shader* shader = nullptr;
    kame::ogl21::Shader* shaderShadow = nullptr;

    kame::squirtle::Node* root = nullptr;
    kame::squirtle::CameraNode* currentCamera = nullptr;

    std::vector<kame::squirtle::LightNode> lights;

    void initSquirtle();
    void shutdownSqurtile();
    void updateNodes(float dt);
    void drawNodes(int viewportWidth, int viewportHeight);
};

} // namespace kame::squirtle

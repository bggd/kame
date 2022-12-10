#pragma once

#include "node.hpp"
#include <kame/math/math.hpp>

enum SquirtleLightNodeType {
    kSquirtlePointLight,
    kSquirtleDirectionalLight
};

namespace kame::squirtle {

struct LightNode : kame::squirtle::Node {

    SquirtleLightNodeType lightType = kSquirtlePointLight;

    bool useShadow = false;
    kame::math::Vector3f direction = kame::math::Vector3f::Zero();
    kame::math::Vector3f diffuse = kame::math::Vector3f::One();

    float constantAttenuation = 1.0f;
    float linearAttenuation = 0.14f;
    float quadraticAttenuation = 0.07f;

    float nearPlane = 1.0f;
    float farPlane = 20.0f;

    SquirtleNodeType getType() const override
    {
        return kSquirtleLightNode;
    }

    SquirtleLightNodeType getLightType() const
    {
        return lightType;
    }
};

} // namespace kame::squirtle

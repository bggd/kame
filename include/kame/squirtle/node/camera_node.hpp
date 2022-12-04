#pragma once

#include "node.hpp"
#include <kame/math/math.hpp>

namespace kame::squirtle {

struct CameraNode : kame::squirtle::Node {

    float fov;
    float aspectRatio;
    float nearPlaneDistance;
    float farPlaneDistance;

    SquirtleNodeType getType() const override
    {
        return kSquirtleCameraNode;
    }

    virtual kame::math::Matrix4x4f getViewMatrix()
    {
        return kame::math::Matrix4x4f::Identity();
    }

    kame::math::Matrix4x4f getPerspectiveMatrix()
    {
        float y_fov = kame::math::helper::toRadians(fov);
        return kame::math::Matrix4x4f::createPerspectiveFieldOfView_RH_NO(y_fov, aspectRatio, nearPlaneDistance, farPlaneDistance);
    }
};

} // namespace kame::squirtle

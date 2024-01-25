#pragma once

#include <kame/kame.hpp>

namespace kame::squirtle {

struct CameraOrbit {
    kame::math::Matrix modelMtx;
    kame::math::Matrix viewMtx;
    kame::math::Matrix projMtx;
    int prevMouseX = 0;
    int prevMouseY = 0;
    float sensitivity = 0.4f;
    kame::math::Vector2 rot = kame::math::Vector2::zero();
    kame::math::Vector2 pan = kame::math::Vector2::zero();
    float zoom = 1.0f;

    // hor+
    float targetHorizontalFov;
    float targetWidth;
    float targetHeight;
    float verticalFov;

    CameraOrbit(float horizontalFov, float width, float height);

    void update(kame::sdl::State& state);

    const kame::math::Matrix& getModelMatrix() const { return modelMtx; };
    const kame::math::Matrix& getViewMatrix() const { return viewMtx; };
    const kame::math::Matrix& getProjectionMatrix() const { return projMtx; };
};

} // namespace kame::squirtle

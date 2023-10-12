#pragma once

namespace kame::math::helper {

const float Pi = 3.14159265358979323846F;

static inline float toRadians(float degree)
{
    return degree * (3.14159265358979323846F / 180.0F);
}

} // namespace kame::math::helper

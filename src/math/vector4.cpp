#include <kame/math/math.hpp>

kame::math::Vector4 kame::math::Vector4::transform(kame::math::Vector4 v, const kame::math::Matrix& m)
{
    float x = (v.x * m.m11) + (v.y * m.m21) + (v.z * m.m31) + (v.w * m.m41);
    float y = (v.x * m.m12) + (v.y * m.m22) + (v.z * m.m32) + (v.w * m.m42);
    float z = (v.x * m.m13) + (v.y * m.m23) + (v.z * m.m33) + (v.w * m.m43);
    float w = (v.x * m.m14) + (v.y * m.m24) + (v.z * m.m34) + (v.w * m.m44);
    return {x, y, z, w};
}

kame::math::Vector4 kame::math::Vector4::lerp(kame::math::Vector4 a, kame::math::Vector4 b, float amount)
{
    return (a * (1.0f - amount)) + (b * amount);
}
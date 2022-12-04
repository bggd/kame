#include <kame/math/math.hpp>

kame::math::Vector3f kame::math::Vector3f::transform(kame::math::Vector3f v, const kame::math::Matrix4x4f& m)
{
    float x = (v.x * m.m11) + (v.y * m.m21) + (v.z * m.m31) + m.m41;
    float y = (v.x * m.m12) + (v.y * m.m22) + (v.z * m.m32) + m.m42;
    float z = (v.x * m.m13) + (v.y * m.m23) + (v.z * m.m33) + m.m43;
    return {x, y, z};
}

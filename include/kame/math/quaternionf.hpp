#pragma once

#include "vector3f.hpp"
#include "vector4f.hpp"

namespace kame::math {

struct Quaternionf {
    float x, y, z, w;

    Quaternionf() {}
    Quaternionf(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}
    Quaternionf(Vector3f v, float W) : x(v.x), y(v.y), z(v.z), w(W) {}
    Quaternionf(Vector4f v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    Quaternionf operator-()
    {
        return {-x, -y, -z, -w};
    }

    static Quaternionf zero()
    {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }

    static Quaternionf identity()
    {
        return {0.0f, 0.0f, 0.0f, 1.0f};
    }

    static float dot(Quaternionf a, Quaternionf b)
    {
        float dot = 0.0f;
        dot += a.x * b.x;
        dot += a.y * b.y;
        dot += a.z * b.z;
        dot += a.w * b.w;

        return dot;
    }

    static float lengthSquared(Quaternionf q)
    {
        return dot(q, q);
    }

    static float length(Quaternionf q)
    {
        return sqrtf(lengthSquared(q));
    }

    static Quaternionf normalize(Quaternionf q)
    {
        Quaternionf n = Quaternionf::zero();
        float len = 1.0F / length(q);
        n.x = q.x * len;
        n.y = q.y * len;
        n.z = q.z * len;
        n.w = q.w * len;
        return n;
    }

    static Quaternionf slerp(Quaternionf a, Quaternionf b, float amount);
};

static Quaternionf operator+(Quaternionf a, Quaternionf b)
{
    return Quaternionf(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

static Quaternionf operator-(Quaternionf a, Quaternionf b)
{
    return Quaternionf(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

static Quaternionf operator*(Quaternionf a, Quaternionf b)
{
    // clang-format off
    return Quaternionf(
        ( b.x * a.w) + (b.y * a.z) - (b.z * a.y) + (b.w * a.x),
        (-b.x * a.z) + (b.y * a.w) + (b.z * a.x) + (b.w * a.y),
        ( b.x * a.y) - (b.y * a.x) + (b.z * a.w) + (b.w * a.z),
        (-b.x * a.x) - (b.y * a.y) - (b.z * a.z) + (b.w * a.w));
    // clang-format on
}

static Quaternionf operator*(Quaternionf a, float scalar)
{
    return Quaternionf(a.x * scalar, a.y * scalar, a.z * scalar, a.w * scalar);
}

} // namespace kame::math

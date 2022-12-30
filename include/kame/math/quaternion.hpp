#pragma once

#include "vector3f.hpp"
#include "vector4f.hpp"

namespace kame::math {

struct Quaternion {
    float x, y, z, w;

    Quaternion() {}
    Quaternion(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}
    Quaternion(Vector3f v, float W) : x(v.x), y(v.y), z(v.z), w(W) {}
    Quaternion(Vector4f v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    Quaternion operator-()
    {
        return {-x, -y, -z, -w};
    }

    static Quaternion zero()
    {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }

    static Quaternion identity()
    {
        return {0.0f, 0.0f, 0.0f, 1.0f};
    }

    static float dot(Quaternion a, Quaternion b)
    {
        float dot = 0.0f;
        dot += a.x * b.x;
        dot += a.y * b.y;
        dot += a.z * b.z;
        dot += a.w * b.w;

        return dot;
    }

    static float lengthSquared(Quaternion q)
    {
        return dot(q, q);
    }

    static float length(Quaternion q)
    {
        return sqrtf(lengthSquared(q));
    }

    static Quaternion normalize(Quaternion q)
    {
        Quaternion n = Quaternion::zero();
        float len = 1.0F / length(q);
        n.x = q.x * len;
        n.y = q.y * len;
        n.z = q.z * len;
        n.w = q.w * len;
        return n;
    }

    static Quaternion slerp(Quaternion a, Quaternion b, float amount);
};

static Quaternion operator+(Quaternion a, Quaternion b)
{
    return Quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

static Quaternion operator-(Quaternion a, Quaternion b)
{
    return Quaternion(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

static Quaternion operator*(Quaternion a, Quaternion b)
{
    // clang-format off
    return Quaternion(
        ( b.x * a.w) + (b.y * a.z) - (b.z * a.y) + (b.w * a.x),
        (-b.x * a.z) + (b.y * a.w) + (b.z * a.x) + (b.w * a.y),
        ( b.x * a.y) - (b.y * a.x) + (b.z * a.w) + (b.w * a.z),
        (-b.x * a.x) - (b.y * a.y) - (b.z * a.z) + (b.w * a.w));
    // clang-format on
}

static Quaternion operator*(Quaternion a, float scalar)
{
    return Quaternion(a.x * scalar, a.y * scalar, a.z * scalar, a.w * scalar);
}

} // namespace kame::math

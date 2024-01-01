#pragma once

#include "vector3.hpp"

namespace kame::math {
struct Matrix;
} // namespace kame::math

#include <cmath>

namespace kame::math {

struct Vector4 {
    float x, y, z, w;

    Vector4() {}
    Vector4(float valueForAll) : x(valueForAll), y(valueForAll), z(valueForAll), w(valueForAll) {}
    Vector4(Vector3 xyz, float w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
    Vector4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}

    Vector4 operator-() const
    {
        return Vector4(-x, -y, -z, -w);
    }

    static Vector4 zero()
    {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }

    static Vector4 one()
    {
        return {1.0f, 1.0f, 1.0f, 1.0f};
    }

    static float dot(Vector4 a, Vector4 b)
    {
        float dot = 0.0f;
        dot += a.x * b.x;
        dot += a.y * b.y;
        dot += a.z * b.z;
        dot += a.w * b.w;

        return dot;
    }

    static float lengthSquared(Vector4 v)
    {
        return dot(v, v);
    }

    static float length(Vector4 v)
    {
        return ::sqrtf(lengthSquared(v));
    }

    static Vector4 normalize(Vector4 v)
    {
        Vector4 n = Vector4::zero();
        float len = 1.0F / length(v);
        n.x = v.x * len;
        n.y = v.y * len;
        n.z = v.z * len;
        n.w = v.w * len;
        return n;
    }

    static Vector4 transform(Vector4 v, const kame::math::Matrix& m);

    static Vector4 lerp(Vector4 a, Vector4 b, float amount);
};

static inline Vector4 operator+(Vector4 a, Vector4 b)
{
    return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

static inline Vector4 operator-(Vector4 a, Vector4 b)
{
    return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

static inline Vector4 operator*(Vector4 a, Vector4 b)
{
    return Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

static inline Vector4 operator/(Vector4 a, Vector4 b)
{
    return Vector4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

static inline Vector4 operator*(Vector4 a, float scalar)
{
    return Vector4(a.x * scalar, a.y * scalar, a.z * scalar, a.w * scalar);
}

static inline Vector4 operator/(Vector4 a, float scalar)
{
    return Vector4(a.x / scalar, a.y / scalar, a.z / scalar, a.w / scalar);
}

} // namespace kame::math

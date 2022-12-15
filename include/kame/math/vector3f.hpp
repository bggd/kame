#pragma once

namespace kame::math {
struct Matrix4x4f;
struct Quaternion;
} // namespace kame::math

#include <cmath>

namespace kame::math {

struct Vector3f {
    float x, y, z;

    Vector3f() {}
    Vector3f(float valueForAll) : x(valueForAll), y(valueForAll), z(valueForAll) {}
    Vector3f(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

    Vector3f operator-()
    {
        return {-x, -y, -z};
    }

    static Vector3f zero()
    {
        return {0.0f, 0.0f, 0.0f};
    }

    static Vector3f one()
    {
        return {1.0f, 1.0f, 1.0f};
    }

    static float dot(Vector3f a, Vector3f b)
    {
        float dot = 0.0f;
        dot += a.x * b.x;
        dot += a.y * b.y;
        dot += a.z * b.z;

        return dot;
    }

    static float lengthSquared(Vector3f v)
    {
        return dot(v, v);
    }

    static float length(Vector3f v)
    {
        return std::sqrtf(lengthSquared(v));
    }

    static Vector3f normalize(Vector3f v)
    {
        Vector3f n = Vector3f::zero();
        float len = 1.0F / length(v);
        n.x = v.x * len;
        n.y = v.y * len;
        n.z = v.z * len;
        return n;
    }

    static Vector3f cross(Vector3f a, Vector3f b)
    {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
    }

    static Vector3f transform(Vector3f v, const kame::math::Matrix4x4f& m);
    static Vector3f transform(Vector3f v, Quaternion q);

    static Vector3f lerp(Vector3f a, Vector3f b, float amount);
};

static Vector3f operator+(Vector3f a, Vector3f b)
{
    return Vector3f(a.x + b.x, a.y + b.y, a.z + b.z);
}

static Vector3f operator-(Vector3f a, Vector3f b)
{
    return Vector3f(a.x - b.x, a.y - b.y, a.z - b.z);
}

static Vector3f operator*(Vector3f a, Vector3f b)
{
    return Vector3f(a.x * b.x, a.y * b.y, a.z * b.z);
}

static Vector3f operator/(Vector3f a, Vector3f b)
{
    return Vector3f(a.x / b.x, a.y / b.y, a.z / b.z);
}

static Vector3f operator*(Vector3f a, float scalar)
{
    return Vector3f(a.x * scalar, a.y * scalar, a.z * scalar);
}

} // namespace kame::math

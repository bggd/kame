#pragma once

namespace kame::math {
struct Matrix;
struct Quaternion;
} // namespace kame::math

#include <cmath>

namespace kame::math {

struct Vector3 {
    float x, y, z;

    Vector3() {}
    Vector3(float valueForAll) : x(valueForAll), y(valueForAll), z(valueForAll) {}
    Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

    Vector3 operator-()
    {
        return {-x, -y, -z};
    }

    static Vector3 zero()
    {
        return {0.0f, 0.0f, 0.0f};
    }

    static Vector3 one()
    {
        return {1.0f, 1.0f, 1.0f};
    }

    static float dot(Vector3 a, Vector3 b)
    {
        float dot = 0.0f;
        dot += a.x * b.x;
        dot += a.y * b.y;
        dot += a.z * b.z;

        return dot;
    }

    static float lengthSquared(Vector3 v)
    {
        return dot(v, v);
    }

    static float length(Vector3 v)
    {
        return ::sqrtf(lengthSquared(v));
    }

    static Vector3 normalize(Vector3 v)
    {
        Vector3 n = Vector3::zero();
        float len = 1.0F / length(v);
        n.x = v.x * len;
        n.y = v.y * len;
        n.z = v.z * len;
        return n;
    }

    static Vector3 cross(Vector3 a, Vector3 b)
    {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
    }

    static Vector3 transform(Vector3 v, const kame::math::Matrix& m);
    static Vector3 transform(Vector3 v, Quaternion q);

    static Vector3 lerp(Vector3 a, Vector3 b, float amount);
};

static inline Vector3 operator+(Vector3 a, Vector3 b)
{
    return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

static inline Vector3 operator-(Vector3 a, Vector3 b)
{
    return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline Vector3 operator*(Vector3 a, Vector3 b)
{
    return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

static inline Vector3 operator/(Vector3 a, Vector3 b)
{
    return Vector3(a.x / b.x, a.y / b.y, a.z / b.z);
}

static inline Vector3 operator*(Vector3 a, float scalar)
{
    return Vector3(a.x * scalar, a.y * scalar, a.z * scalar);
}

static inline Vector3 operator/(Vector3 a, float scalar)
{
    return Vector3(a.x / scalar, a.y / scalar, a.z / scalar);
}

static inline Vector3& operator+=(Vector3& a, Vector3 b)
{
    a = a + b;
    return a;
}

static inline Vector3& operator-=(Vector3& a, Vector3 b)
{
    a = a / b;
    return a;
}

static inline Vector3& operator*=(Vector3& a, Vector3 b)
{
    a = a * b;
    return a;
}

static inline Vector3& operator/=(Vector3& a, Vector3 b)
{
    a = a / b;
    return a;
}

} // namespace kame::math

#pragma once

namespace kame::math {

struct Vector4f {
    float x, y, z, w;

    Vector4f() {}
    Vector4f(float valueForAll) : x(valueForAll), y(valueForAll), z(valueForAll), w(valueForAll) {}
    Vector4f(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}

    Vector4f operator-()
    {
        return Vector4f(-x, -y, -z, -w);
    }

    static Vector4f Zero()
    {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }
};

static Vector4f operator+(Vector4f a, Vector4f b)
{
    return Vector4f(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

static Vector4f operator-(Vector4f a, Vector4f b)
{
    return Vector4f(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

static Vector4f operator*(Vector4f a, Vector4f b)
{
    return Vector4f(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

static Vector4f operator/(Vector4f a, Vector4f b)
{
    return Vector4f(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

} // namespace kame::math

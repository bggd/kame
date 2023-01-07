#pragma once

namespace kame::math {

struct Vector4 {
    float x, y, z, w;

    Vector4() {}
    Vector4(float valueForAll) : x(valueForAll), y(valueForAll), z(valueForAll), w(valueForAll) {}
    Vector4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}

    Vector4 operator-()
    {
        return Vector4(-x, -y, -z, -w);
    }

    static Vector4 zero()
    {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }

    static Vector4 transform(Vector4 v, const kame::math::Matrix& m);
};

static Vector4 operator+(Vector4 a, Vector4 b)
{
    return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

static Vector4 operator-(Vector4 a, Vector4 b)
{
    return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

static Vector4 operator*(Vector4 a, Vector4 b)
{
    return Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

static Vector4 operator/(Vector4 a, Vector4 b)
{
    return Vector4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

} // namespace kame::math

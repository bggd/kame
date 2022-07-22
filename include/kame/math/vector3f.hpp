#pragma once

namespace kame::math {

struct Vector3f {
    float x, y, z;

    Vector3f() {}
    Vector3f(float valueForAll) : x(valueForAll), y(valueForAll), z(valueForAll) {}
    Vector3f(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

    Vector3f operator-()
    {
        return Vector3f(-x, -y, -z);
    }
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

} // namespace kame::math
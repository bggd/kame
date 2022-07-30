#pragma once

namespace kame::math {

struct Vector2f {
    float x, y;

    Vector2f() {}
    Vector2f(float valueForAll) : x(valueForAll), y(valueForAll) {}
    Vector2f(float X, float Y) : x(X), y(Y) {}

    Vector2f operator-()
    {
        return Vector2f(-x, -y);
    }
};

static Vector2f operator+(Vector2f a, Vector2f b)
{
    return Vector2f(a.x + b.x, a.y + b.y);
}

static Vector2f operator-(Vector2f a, Vector2f b)
{
    return Vector2f(a.x - b.x, a.y - b.y);
}

static Vector2f operator*(Vector2f a, Vector2f b)
{
    return Vector2f(a.x * b.x, a.y * b.y);
}

static Vector2f operator/(Vector2f a, Vector2f b)
{
    return Vector2f(a.x / b.x, a.y / b.y);
}

} // namespace kame::math

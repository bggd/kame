#pragma once

namespace kame::math {

struct Vector2 {
    float x, y;

    Vector2() {}
    Vector2(float valueForAll) : x(valueForAll), y(valueForAll) {}
    Vector2(float X, float Y) : x(X), y(Y) {}

    Vector2 operator-()
    {
        return Vector2(-x, -y);
    }

    static Vector2 zero()
    {
        return {0.0f, 0.0f};
    }

    static Vector2 one()
    {
        return {1.0f, 1.0f};
    }
};

static inline Vector2 operator+(Vector2 a, Vector2 b)
{
    return Vector2(a.x + b.x, a.y + b.y);
}

static inline Vector2 operator-(Vector2 a, Vector2 b)
{
    return Vector2(a.x - b.x, a.y - b.y);
}

static inline Vector2 operator*(Vector2 a, Vector2 b)
{
    return Vector2(a.x * b.x, a.y * b.y);
}

static inline Vector2 operator/(Vector2 a, Vector2 b)
{
    return Vector2(a.x / b.x, a.y / b.y);
}

} // namespace kame::math

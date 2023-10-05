#pragma once

#include "types.hpp"

namespace kame::love2d::physics {

void setMeter(float scale);
float getMeter();

World newWorld(float xg = 0.0f, float yg = 0.0f, bool sleep = true);
Body newBody(World world, float x = 0.0f, float y = 0.0f, const char* type = "static");
PolygonShape newPolygonShape(std::vector<float>& vertices);
PolygonShape newPolygonShape(std::vector<float> vertices);
template <typename... Args>
PolygonShape newPolygonShape(Args... args)
{
    return newPolygonShape({args...});
}
PolygonShape newRectangleShape(float width, float height);
PolygonShape newRectangleShape(float x, float y, float width, float height, float angle = 0.0f);
Fixture newFixture(Body body, const PolygonShape& shape, float density = 1.0f);

} // namespace kame::love2d::physics

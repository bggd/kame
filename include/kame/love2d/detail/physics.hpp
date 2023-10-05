#pragma once

#include <box2d/box2d.h>

#include <vector>
#include <string>
#include <memory>

namespace kame::love2d::detail::physics {

struct World {
    b2World* world = nullptr;

    virtual ~World();
    bool release();

    void update(float dt, int velocityiterations = 8, int positioniterations = 3);
};

struct Body {
    b2Body* body = nullptr;

    virtual ~Body();
    bool release();

    std::vector<float> getWorldPoints(std::vector<float> vertices);

    float getX();
    float getY();
};

struct Shape {
    virtual ~Shape() {}
    virtual const std::string getType() = 0;
};

struct PolygonShape : Shape {
    b2PolygonShape polygonShape;

    virtual ~PolygonShape() override {}

    const std::string getType() override
    {
        return "polygon";
    }

    std::vector<float> getPoints() const;
};
} // namespace kame::love2d::detail::physics

namespace kame::love2d {
using Shape = kame::love2d::detail::physics::Shape;
}

namespace kame::love2d::detail::physics {

struct Fixture {
    b2Fixture* fixture = nullptr;
    PolygonShape shape;

    ~Fixture();
    bool release();

    const kame::love2d::Shape* getShape();
};

struct Physics {
    float meter = 30.0f;
    std::vector<b2Fixture*> destroyQueueFixture;
    std::vector<b2Body*> destroyQueueBody;
    std::vector<b2World*> destroyQueueWorld;

    void destroyQueues();

    void setMeter(float scale);
    float getMeter();
    b2Vec2 scaleUp(b2Vec2 v);
    b2Vec2 scaleDown(b2Vec2 v);
    float scaleUp(float v);
    float scaleDown(float v);

    World* newWorld(float xg = 0.0f, float yg = 0.0f, bool sleep = true);
    Body* newBody(World* world, float x = 0.0f, float y = 0.0f, const char* type = "static");
    PolygonShape newPolygonShape(std::vector<float>& vertices);
    PolygonShape newRectangleShape(float width, float height);
    PolygonShape newRectangleShape(float x, float y, float width, float height, float angle = 0.0f);
    Fixture* newFixture(Body* body, const PolygonShape& shape, float density = 1.0f);
};

} // namespace kame::love2d::detail::physics

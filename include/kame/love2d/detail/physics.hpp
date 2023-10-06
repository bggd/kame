#pragma once

#include <box2d/box2d.h>

#include <memory>
#include <vector>
#include <string>
#include <variant>

namespace kame::love2d::detail::physics {

struct World {
    b2World* world = nullptr;

    virtual ~World();
    bool release();

    void update(float dt, int velocityiterations = 8, int positioniterations = 3);
    void debugDraw();
};

struct Body {
    b2Body* body = nullptr;
    std::shared_ptr<kame::love2d::detail::physics::World> pWorld;

    virtual ~Body();
    bool release();

    std::vector<float> getWorldPoints(std::vector<float> vertices);

    float getX();
    float getY();
};

struct Shape {
    virtual ~Shape() {}
    virtual const std::string getType() const
    {
        return "";
    }
};

struct CircleShape : Shape {
    b2CircleShape circleShape;

    virtual ~CircleShape() override {}
    const std::string getType() const override
    {
        return "circle";
    }
};

struct PolygonShape : Shape {
    b2PolygonShape polygonShape;

    virtual ~PolygonShape() override {}

    const std::string getType() const override
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

struct DebugDraw : b2Draw {
    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
    void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;
    void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;
    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
    void DrawTransform(const b2Transform& xf) override;
    void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;
};

struct Fixture {
    b2Fixture* fixture = nullptr;
    std::shared_ptr<kame::love2d::detail::physics::Body> pBody;
    std::variant<kame::love2d::detail::physics::CircleShape, kame::love2d::detail::physics::PolygonShape> shape;

    ~Fixture();
    bool release();

    const kame::love2d::Shape* getShape();
};

struct Physics {
    float meter = 30.0f;

    static std::vector<b2Fixture*> destroyQueueFixture;
    static std::vector<b2Body*> destroyQueueBody;
    static std::vector<b2World*> destroyQueueWorld;
    static void destroyQueues();
    static size_t createdWorldCount;
    static size_t deletedWorldCount;
    static size_t createdBodyCount;
    static size_t deletedBodyCount;
    static size_t createdFixtureCount;
    static size_t deletedFixtureCount;
    static DebugDraw debugDraw;

    void setMeter(float scale);
    float getMeter();
    b2Vec2 scaleUp(b2Vec2 v);
    b2Vec2 scaleDown(b2Vec2 v);
    float scaleUp(float v);
    float scaleDown(float v);

    World* newWorld(float xg = 0.0f, float yg = 0.0f, bool sleep = true);
    Body* newBody(World* world, float x = 0.0f, float y = 0.0f, const char* type = "static");
    CircleShape newCircleShape(float radius);
    PolygonShape newPolygonShape(std::vector<float>& vertices);
    PolygonShape newRectangleShape(float width, float height);
    PolygonShape newRectangleShape(float x, float y, float width, float height, float angle = 0.0f);
    Fixture* newFixture(Body* body, const CircleShape& shape, float density = 1.0f);
    Fixture* newFixture(Body* body, const PolygonShape& shape, float density = 1.0f);
};

} // namespace kame::love2d::detail::physics

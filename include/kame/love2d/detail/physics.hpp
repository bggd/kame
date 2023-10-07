#pragma once

#include <box2d/box2d.h>

#include <memory>
#include <vector>
#include <string>
#include <variant>
#include <unordered_map>
#include <functional>

namespace kame::love2d::detail::physics {

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
    std::shared_ptr<void> pBody;
    std::variant<kame::love2d::detail::physics::CircleShape, kame::love2d::detail::physics::PolygonShape> shape;

    ~Fixture();
    bool release();

    const kame::love2d::Shape* getShape();
};

struct Contact {
    b2Contact* contact = nullptr;
};

} // namespace kame::love2d::detail::physics

namespace kame::love2d {
using Fixture = std::shared_ptr<kame::love2d::detail::physics::Fixture>;
using Contact = kame::love2d::detail::physics::Contact;
} // namespace kame::love2d

namespace kame::love2d::detail::physics {

using CollisionCallbackContact = std::function<void(kame::love2d::Fixture, kame::love2d::Fixture, kame::love2d::Contact)>;
using CollisionCallbackContactPostResolve = std::function<void(kame::love2d::Fixture, kame::love2d::Fixture, kame::love2d::Contact, const std::vector<std::pair<float, float>>&)>;

struct ContactListener : b2ContactListener {
    kame::love2d::detail::physics::CollisionCallbackContact _beginContact;
    kame::love2d::detail::physics::CollisionCallbackContact _endContact;
    kame::love2d::detail::physics::CollisionCallbackContact _preSolve;
    kame::love2d::detail::physics::CollisionCallbackContactPostResolve _postSolve;

    ~ContactListener() {}

    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
};

struct World {
    b2World* world = nullptr;
    ContactListener listener;

    virtual ~World();
    bool release();

    void update(float dt, int velocityiterations = 8, int positioniterations = 3);
    void setCallback(
        kame::love2d::detail::physics::CollisionCallbackContact beginContact, kame::love2d::detail::physics::CollisionCallbackContact endContact = [](love2d::Fixture, love2d::Fixture, love2d::detail::physics::Contact) {}, kame::love2d::detail::physics::CollisionCallbackContact preSolve = [](love2d::Fixture, love2d::Fixture, love2d::detail::physics::Contact) {}, kame::love2d::detail::physics::CollisionCallbackContactPostResolve postSolve = [](love2d::Fixture, love2d::Fixture, love2d::detail::physics::Contact, const std::vector<std::pair<float, float>>&) {});
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
    std::unordered_map<b2Fixture*, kame::love2d::Fixture::weak_type> fixtureMap;
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

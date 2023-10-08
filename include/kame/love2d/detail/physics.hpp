#pragma once

#include <kame/love2d/detail/box2d.hpp>

namespace kame::love2d {
using Contact = kame::love2d::detail::box2d::Box2dContact;

using Fixture = kame::love2d::detail::box2d::SPtrBox2dFixture;

using CollisionCallbackContact = std::function<void(kame::love2d::Fixture, kame::love2d::Fixture, const kame::love2d::Contact*)>;
using CollisionCallbackContactPostResolve = std::function<void(kame::love2d::Fixture, kame::love2d::Fixture, const kame::love2d::Contact*, const std::vector<std::pair<float, float>>&)>;
} // namespace kame::love2d

namespace kame::love2d::detail::physics {

struct ContactListener : b2ContactListener {
    kame::love2d::CollisionCallbackContact _beginContact;
    kame::love2d::CollisionCallbackContact _endContact;
    kame::love2d::CollisionCallbackContact _preSolve;
    kame::love2d::CollisionCallbackContactPostResolve _postSolve;

    ~ContactListener() {}

    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
};

struct World {
    kame::love2d::detail::box2d::SPtrBox2dWorld _world;
    ContactListener listener;
    void setCallback(kame::love2d::CollisionCallbackContact beginContact, kame::love2d::CollisionCallbackContact endContact, kame::love2d::CollisionCallbackContact preSolve, kame::love2d::CollisionCallbackContactPostResolve postSolve);
};
using SPtrWorld = std::shared_ptr<World>;

struct Physics {
    float meter = 30.0f;

    void setMeter(float scale);
    float getMeter();
    b2Vec2 scaleUp(b2Vec2 v);
    b2Vec2 scaleDown(b2Vec2 v);
    float scaleUp(float v);
    float scaleDown(float v);

    SPtrWorld newWorld(float xg = 0.0f, float yg = 0.0f, bool sleep = true);
    kame::love2d::detail::box2d::SPtrBox2dBody newBody(kame::love2d::detail::box2d::SPtrBox2dWorld world, float x = 0.0f, float y = 0.0f, const char* type = "static");
    kame::love2d::detail::box2d::Box2dCircleShape newCircleShape(float radius);
    kame::love2d::detail::box2d::Box2dPolygonShape newPolygonShape(std::vector<float>& vertices);
    kame::love2d::detail::box2d::Box2dPolygonShape newRectangleShape(float width, float height);
    kame::love2d::detail::box2d::Box2dPolygonShape newRectangleShape(float x, float y, float width, float height, float angle = 0.0f);
    kame::love2d::detail::box2d::SPtrBox2dFixture newFixture(kame::love2d::detail::box2d::SPtrBox2dBody body, const kame::love2d::detail::box2d::Box2dCircleShape& shape, float density = 1.0f);
    kame::love2d::detail::box2d::SPtrBox2dFixture newFixture(kame::love2d::detail::box2d::SPtrBox2dBody body, const kame::love2d::detail::box2d::Box2dPolygonShape& shape, float density = 1.0f);
};

} // namespace kame::love2d::detail::physics

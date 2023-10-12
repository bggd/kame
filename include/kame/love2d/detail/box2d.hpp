#pragma once

#include <any>
#include "box2d_header.hpp"
#include <memory>
#include <variant>
#include <vector>
#include <functional>

namespace kame::love2d::detail::box2d {

struct Box2dCircleShape {
    b2CircleShape _circleShapeB2D;

    float getRadius();
};

struct Box2dPolygonShape {
    b2PolygonShape _polygonShapeB2D;
    std::vector<float> _points;

    const std::vector<float>& getPoints();
};

using Box2dShape = std::variant<std::monostate, Box2dCircleShape, Box2dPolygonShape>;

struct Box2dContact {
    b2Contact* _contactB2D = nullptr;

    std::pair<float, float> getNormal() const;
};

struct Box2dWorld {
    b2World* _worldB2D = nullptr;
    std::any _listener;

    virtual ~Box2dWorld();

    void update(float dt, int velocityiterations, int positioniterations);
    void debugDraw();
};
using SPtrBox2dWorld = std::shared_ptr<Box2dWorld>;

struct Box2dBody {
    SPtrBox2dWorld _parentWorld;
    b2Body* _bodyB2D = nullptr;

    virtual ~Box2dBody();

    void applyForce(float fx, float fy);

    float getAngle();
    float getX();
    float getY();
    std::pair<float, float> getPosition() { return {getX(), getY()}; }
    std::vector<float> getWorldPoints(const std::vector<float>& points);

    void setAngle(float radius);
    void setLinearVelocity(float x, float y);
    void setPosition(float x, float y);
};
using SPtrBox2dBody = std::shared_ptr<Box2dBody>;

struct Box2dFixture {
    SPtrBox2dBody _parentBody;
    b2Fixture* _fixtureB2D = nullptr;

    virtual ~Box2dFixture();

    float getRestitution();
    void setRestitution(float restitution);
};
using SPtrBox2dFixture = std::shared_ptr<Box2dFixture>;

extern std::unordered_map<b2Fixture*, kame::love2d::detail::box2d::SPtrBox2dFixture::weak_type> fixtureMap;

SPtrBox2dWorld newSPtrBox2dWorld(b2Vec2 gravity, bool sleep = true);

SPtrBox2dBody newSPtrBox2dBody(SPtrBox2dWorld w, float x = 0.0f, float y = 0.0f, b2BodyType type = b2BodyType::b2_staticBody);

SPtrBox2dFixture newSPtrBox2DFixture(SPtrBox2dBody body, const Box2dShape& shape, float density = 1.0f);

struct DebugDraw : b2Draw {
    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
    void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;
    void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;
    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
    void DrawTransform(const b2Transform& xf) override;
    void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;
};

} // namespace kame::love2d::detail::box2d

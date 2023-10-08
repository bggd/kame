#pragma once

#include <box2d/box2d.h>
#include <memory>
#include <variant>
#include <vector>
#include <functional>

namespace kame::love2d::detail::box2d {

struct Box2dCircleShape {
    b2CircleShape _circleShapeB2D;
};

struct Box2dPolygonShape {
    b2PolygonShape _polygonShapeB2D;
};

using Box2dShape = std::variant<Box2dCircleShape, Box2dPolygonShape>;

struct Box2dContact {
    b2Contact* _contactB2D = nullptr;

    std::pair<float, float> getNormal() const;
};

struct Box2dWorld {
    b2World* _worldB2D = nullptr;

    virtual ~Box2dWorld();

    void update(float dt, int velocityiterations, int positioniterations);
};
using SPtrBox2dWorld = std::shared_ptr<Box2dWorld>;

struct Box2dBody {
    SPtrBox2dWorld parentWorld;
    b2Body* _bodyB2D = nullptr;

    virtual ~Box2dBody();

    float getX();
    float getY();
};
using SPtrBox2dBody = std::shared_ptr<Box2dBody>;

struct Box2dFixture {
    SPtrBox2dBody parentBody;
    b2Fixture* _fixtureB2D = nullptr;

    virtual ~Box2dFixture();
};
using SPtrBox2dFixture = std::shared_ptr<Box2dFixture>;

extern std::unordered_map<b2Fixture*, kame::love2d::detail::box2d::SPtrBox2dFixture::weak_type> fixtureMap;

SPtrBox2dWorld newSPtrBox2dWorld(b2Vec2 gravity, bool sleep = true);

SPtrBox2dBody newSPtrBox2dBody(SPtrBox2dWorld w, float x = 0.0f, float y = 0.0f, b2BodyType type = b2BodyType::b2_staticBody);

SPtrBox2dFixture newSPtrBox2DFixture(SPtrBox2dBody body, const Box2dShape& shape, float density = 1.0f);

} // namespace kame::love2d::detail::box2d

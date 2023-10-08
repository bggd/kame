#include <all.hpp>

void kame::love2d::detail::physics::ContactListener::BeginContact(b2Contact* contact)
{
    if (!kame::kameIsInit())
    {
        return;
    }

    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    auto& fixtureMap = kame::love2d::detail::box2d::fixtureMap;

    assert(fixtureMap.contains(contact->GetFixtureA()));
    kame::love2d::Fixture::weak_type fixture_a = fixtureMap[contact->GetFixtureA()];
    assert(fixtureMap.contains(contact->GetFixtureB()));
    kame::love2d::Fixture::weak_type fixture_b = fixtureMap[contact->GetFixtureB()];

    kame::love2d::Contact c;
    c._contactB2D = contact;

    assert(!fixture_a.expired());
    if (kame::love2d::Fixture a = fixture_a.lock())
    {
        assert(!fixture_b.expired());
        if (kame::love2d::Fixture b = fixture_b.lock())
        {
            _beginContact(a, b, &c);
        }
    }
}

void kame::love2d::detail::physics::ContactListener::EndContact(b2Contact* contact)
{
    if (!kame::kameIsInit())
    {
        return;
    }

    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    auto& fixtureMap = kame::love2d::detail::box2d::fixtureMap;

    assert(fixtureMap.contains(contact->GetFixtureA()));
    kame::love2d::Fixture::weak_type fixture_a = fixtureMap[contact->GetFixtureA()];
    assert(fixtureMap.contains(contact->GetFixtureB()));
    kame::love2d::Fixture::weak_type fixture_b = fixtureMap[contact->GetFixtureB()];

    kame::love2d::Contact c;
    c._contactB2D = contact;

    kame::love2d::Fixture a;

    assert(!fixture_a.expired());
    if (kame::love2d::Fixture a = fixture_a.lock())
    {
        assert(!fixture_b.expired());
        if (kame::love2d::Fixture b = fixture_b.lock())
        {
            _endContact(a, b, &c);
        }
    }
}

void kame::love2d::detail::physics::ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    B2_NOT_USED(oldManifold);

    if (!kame::kameIsInit())
    {
        return;
    }

    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    auto& fixtureMap = kame::love2d::detail::box2d::fixtureMap;

    assert(fixtureMap.contains(contact->GetFixtureA()));
    kame::love2d::Fixture::weak_type fixture_a = fixtureMap[contact->GetFixtureA()];
    assert(fixtureMap.contains(contact->GetFixtureB()));
    kame::love2d::Fixture::weak_type fixture_b = fixtureMap[contact->GetFixtureB()];

    kame::love2d::Contact c;
    c._contactB2D = contact;

    assert(!fixture_a.expired());
    if (kame::love2d::Fixture a = fixture_a.lock())
    {
        assert(!fixture_b.expired());
        if (kame::love2d::Fixture b = fixture_b.lock())
        {
            _preSolve(a, b, &c);
        }
    }
}

void kame::love2d::detail::physics::ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    if (!kame::kameIsInit())
    {
        return;
    }

    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    auto& fixtureMap = kame::love2d::detail::box2d::fixtureMap;

    assert(fixtureMap.contains(contact->GetFixtureA()));
    kame::love2d::Fixture::weak_type fixture_a = fixtureMap[contact->GetFixtureA()];
    assert(fixtureMap.contains(contact->GetFixtureB()));
    kame::love2d::Fixture::weak_type fixture_b = fixtureMap[contact->GetFixtureB()];

    kame::love2d::Contact c;
    c._contactB2D = contact;

    assert(!fixture_a.expired());
    if (kame::love2d::Fixture a = fixture_a.lock())
    {
        assert(!fixture_b.expired());
        if (kame::love2d::Fixture b = fixture_b.lock())
        {
            static std::vector<std::pair<float, float>> contactImpulse;
            contactImpulse.reserve(impulse->count);
            contactImpulse.clear();
            for (int i = 0; i < impulse->count; ++i)
            {
                contactImpulse.emplace_back(impulse->normalImpulses[i], impulse->tangentImpulses[i]);
            }
            _postSolve(a, b, &c, contactImpulse);
        }
    }
}

void kame::love2d::detail::physics::World::update(float dt, int velocityiterations, int positioniterations)
{
    _world->_worldB2D->Step(dt, velocityiterations, positioniterations);
}

void kame::love2d::detail::physics::World::setCallback(kame::love2d::CollisionCallbackContact beginContact, kame::love2d::CollisionCallbackContact endContact, kame::love2d::CollisionCallbackContact preSolve, kame::love2d::CollisionCallbackContactPostResolve postSolve)
{
    listener._beginContact = beginContact;
    listener._endContact = endContact;
    listener._preSolve = preSolve;
    listener._postSolve = postSolve;
    _world->_worldB2D->SetContactListener(&listener);
}

void kame::love2d::detail::physics::Physics::setMeter(float scale)
{
    assert(scale >= 1.0f);
    meter = scale;
}

float kame::love2d::detail::physics::Physics::getMeter()
{
    return meter;
}

b2Vec2 kame::love2d::detail::physics::Physics::scaleUp(b2Vec2 v)
{
    return b2Vec2(v.x * meter, v.y * meter);
}

b2Vec2 kame::love2d::detail::physics::Physics::scaleDown(b2Vec2 v)
{
    return b2Vec2(v.x / meter, v.y / meter);
}

float kame::love2d::detail::physics::Physics::scaleUp(float v)
{
    return v * meter;
}

float kame::love2d::detail::physics::Physics::scaleDown(float v)
{
    return v / meter;
}

kame::love2d::detail::physics::SPtrWorld kame::love2d::detail::physics::Physics::newWorld(float xg, float yg, bool sleep)
{
    auto* p = new kame::love2d::detail::physics::World();
    assert(p);
    SPtrWorld w = std::shared_ptr<World>(p);
    w->_world = kame::love2d::detail::box2d::newSPtrBox2dWorld(scaleDown(b2Vec2(xg, yg)), sleep);
    return w;
}

kame::love2d::detail::box2d::SPtrBox2dBody kame::love2d::detail::physics::Physics::newBody(kame::love2d::detail::box2d::SPtrBox2dWorld world, float x, float y, const char* type)
{
    assert(type);
    assert(std::string(type) == "static" || std::string(type) == "dynamic" || std::string(type) == "kinematic");

    b2BodyType t = b2BodyType::b2_staticBody;
    if (std::string(type) == "dynamic")
    {
        t = b2BodyType::b2_dynamicBody;
    }
    else if (std::string(type) == "kinematic")
    {
        t = b2BodyType::b2_kinematicBody;
    }

    return kame::love2d::detail::box2d::newSPtrBox2dBody(world, scaleDown(x), scaleDown(y), t);
}

kame::love2d::detail::box2d::Box2dCircleShape kame::love2d::detail::physics::Physics::newCircleShape(float radius)
{
    kame::love2d::detail::box2d::Box2dCircleShape cs;
    cs._circleShapeB2D = b2CircleShape();
    cs._circleShapeB2D.m_radius = scaleDown(radius);
    return cs;
}

kame::love2d::detail::box2d::Box2dPolygonShape kame::love2d::detail::physics::Physics::newPolygonShape(std::vector<float>& vertices)
{
    assert(vertices.size() % 2 == 0);

    size_t vcount = vertices.size() / 2;
    assert(vcount >= 3);
    assert(vcount <= b2_maxPolygonVertices);

    kame::love2d::detail::box2d::Box2dPolygonShape ps;
    ps._polygonShapeB2D = b2PolygonShape();

    std::vector<float> points = vertices;
    for (auto& i : points)
    {
        i = scaleDown(i);
    }

    ps._polygonShapeB2D.Set((b2Vec2*)points.data(), vcount);

    return ps;
}

kame::love2d::detail::box2d::Box2dPolygonShape kame::love2d::detail::physics::Physics::newRectangleShape(float width, float height)
{
    kame::love2d::detail::box2d::Box2dPolygonShape ps;
    ps._polygonShapeB2D = b2PolygonShape();

    ps._polygonShapeB2D.SetAsBox(scaleDown(width / 2.0f), scaleDown(height / 2.0f));

    return ps;
}

kame::love2d::detail::box2d::Box2dPolygonShape kame::love2d::detail::physics::Physics::newRectangleShape(float x, float y, float width, float height, float angle)
{
    kame::love2d::detail::box2d::Box2dPolygonShape ps;
    ps._polygonShapeB2D = b2PolygonShape();

    ps._polygonShapeB2D.SetAsBox(scaleDown(width / 2.0f), scaleDown(height / 2.0f), scaleDown(b2Vec2(x, y)), angle);

    return ps;
}

kame::love2d::detail::box2d::SPtrBox2dFixture kame::love2d::detail::physics::Physics::newFixture(kame::love2d::detail::box2d::SPtrBox2dBody body, const kame::love2d::detail::box2d::Box2dCircleShape& shape, float density)
{
    return kame::love2d::detail::box2d::newSPtrBox2DFixture(body, shape, density);
}

kame::love2d::detail::box2d::SPtrBox2dFixture kame::love2d::detail::physics::Physics::newFixture(kame::love2d::detail::box2d::SPtrBox2dBody body, const kame::love2d::detail::box2d::Box2dPolygonShape& shape, float density)
{
    return kame::love2d::detail::box2d::newSPtrBox2DFixture(body, shape, density);
}

// kame::love2d::detail::physics::DebugDraw kame::love2d::detail::physics::Physics::debugDraw;

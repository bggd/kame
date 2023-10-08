#include <all.hpp>

void kame::love2d::detail::physics::ContactListener::BeginContact(b2Contact* contact)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    assert(ctx.physics->fixtureMap.contains(contact->GetFixtureA()));
    kame::love2d::Fixture::weak_type fixture_a = ctx.physics->fixtureMap[contact->GetFixtureA()];
    assert(ctx.physics->fixtureMap.contains(contact->GetFixtureB()));
    kame::love2d::Fixture::weak_type fixture_b = ctx.physics->fixtureMap[contact->GetFixtureB()];

    assert(!fixture_a.expired());
    assert(!fixture_b.expired());

    kame::love2d::detail::physics::Contact c;
    c.contact = contact;

    kame::love2d::Fixture a = fixture_a.lock();
    kame::love2d::Fixture b = fixture_b.lock();
    _beginContact(a, b, &c);
}

void kame::love2d::detail::physics::ContactListener::EndContact(b2Contact* contact)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    assert(ctx.physics->fixtureMap.contains(contact->GetFixtureA()));
    kame::love2d::Fixture::weak_type fixture_a = ctx.physics->fixtureMap[contact->GetFixtureA()];
    assert(ctx.physics->fixtureMap.contains(contact->GetFixtureB()));
    kame::love2d::Fixture::weak_type fixture_b = ctx.physics->fixtureMap[contact->GetFixtureB()];

    assert(!fixture_a.expired());
    assert(!fixture_b.expired());

    kame::love2d::detail::physics::Contact c;
    c.contact = contact;

    kame::love2d::Fixture a = fixture_a.lock();
    kame::love2d::Fixture b = fixture_b.lock();
    _endContact(a, b, &c);
}

void kame::love2d::detail::physics::ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    B2_NOT_USED(oldManifold);

    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    assert(ctx.physics->fixtureMap.contains(contact->GetFixtureA()));
    kame::love2d::Fixture::weak_type fixture_a = ctx.physics->fixtureMap[contact->GetFixtureA()];
    assert(ctx.physics->fixtureMap.contains(contact->GetFixtureB()));
    kame::love2d::Fixture::weak_type fixture_b = ctx.physics->fixtureMap[contact->GetFixtureB()];

    assert(!fixture_a.expired());
    assert(!fixture_b.expired());

    kame::love2d::detail::physics::Contact c;
    c.contact = contact;

    kame::love2d::Fixture a = fixture_a.lock();
    kame::love2d::Fixture b = fixture_b.lock();
    _preSolve(a, b, &c);
}

void kame::love2d::detail::physics::ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    assert(ctx.physics->fixtureMap.contains(contact->GetFixtureA()));
    kame::love2d::Fixture::weak_type fixture_a = ctx.physics->fixtureMap[contact->GetFixtureA()];
    assert(ctx.physics->fixtureMap.contains(contact->GetFixtureB()));
    kame::love2d::Fixture::weak_type fixture_b = ctx.physics->fixtureMap[contact->GetFixtureB()];

    assert(!fixture_a.expired());
    assert(!fixture_b.expired());

    kame::love2d::detail::physics::Contact c;
    c.contact = contact;

    kame::love2d::Fixture a = fixture_a.lock();
    kame::love2d::Fixture b = fixture_b.lock();

    static std::vector<std::pair<float, float>> contactImpulse;
    contactImpulse.reserve(impulse->count);
    contactImpulse.clear();
    for (int i = 0; i < impulse->count; ++i)
    {
        contactImpulse.emplace_back(impulse->normalImpulses[i], impulse->tangentImpulses[i]);
    }
    _postSolve(a, b, &c, contactImpulse);
}

kame::love2d::detail::physics::World::~World()
{
    if (release())
    {
        return;
    }

    if (world)
    {
        kame::love2d::detail::physics::Physics::destroyQueueWorld.emplace_back(world);
        world->SetContactListener(nullptr);
        world = nullptr;
    }
}

bool kame::love2d::detail::physics::World::release()
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    if (!ctx.isValid())
    {
        return false;
    }

    if (world)
    {
        kame::love2d::detail::physics::Physics::destroyQueueWorld.emplace_back(world);
        world->SetContactListener(nullptr);
        world = nullptr;
        return true;
    }
    else
    {
        return false;
    }
}

kame::love2d::detail::physics::Body::~Body()
{
    if (release())
    {
        return;
    }

    if (body)
    {
        kame::love2d::detail::physics::Physics::destroyQueueBody.emplace_back(body);
        body = nullptr;
    }
}

bool kame::love2d::detail::physics::Body::release()
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    if (!ctx.isValid())
    {
        return false;
    }

    if (body)
    {
        kame::love2d::detail::physics::Physics::destroyQueueBody.emplace_back(body);
        body = nullptr;
        return true;
    }
    else
    {
        return false;
    }
}

kame::love2d::detail::physics::Fixture::~Fixture()
{
    if (release())
    {
        return;
    }

    if (fixture)
    {
        kame::love2d::detail::physics::Physics::destroyQueueFixture.emplace_back(fixture);
        fixture = nullptr;
    }
}

bool kame::love2d::detail::physics::Fixture::release()
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    if (!ctx.isValid())
    {
        return false;
    }

    if (fixture)
    {
        kame::love2d::detail::physics::Physics::destroyQueueFixture.emplace_back(fixture);
        fixture = nullptr;
        return true;
    }
    else
    {
        return false;
    }
}

void kame::love2d::detail::physics::World::update(float dt, int velocityiterations, int positioniterations)
{
    world->Step(dt, velocityiterations, positioniterations);

    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    kame::love2d::detail::physics::Physics::destroyQueues();
}

void kame::love2d::detail::physics::World::setCallback(kame::love2d::detail::physics::CollisionCallbackContact beginContact, kame::love2d::detail::physics::CollisionCallbackContact endContact, kame::love2d::detail::physics::CollisionCallbackContact preSolve, kame::love2d::detail::physics::CollisionCallbackContactPostResolve postSolve)
{
    listener._beginContact = beginContact;
    listener._endContact = endContact;
    listener._preSolve = preSolve;
    listener._postSolve = postSolve;
    world->SetContactListener(&listener);
}

void kame::love2d::detail::physics::World::debugDraw()
{
    world->DebugDraw();
}

std::vector<float> kame::love2d::detail::physics::Body::getWorldPoints(std::vector<float> vertices)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    std::vector<float> points(vertices.size());
    points.clear();

    for (size_t i = 0; i < vertices.size(); i += 2)
    {
        b2Vec2 v = b2Vec2(vertices[i], vertices[i + 1]);
        v = ctx.physics->scaleDown(v);
        v = body->GetWorldPoint(v);
        v = ctx.physics->scaleUp(v);

        points.emplace_back(v.x);
        points.emplace_back(v.y);
    }

    return points;
}

float kame::love2d::detail::physics::Body::getX()
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    return ctx.physics->scaleUp(body->GetPosition().x);
}

float kame::love2d::detail::physics::Body::getY()
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    return ctx.physics->scaleUp(body->GetPosition().y);
}

std::vector<float> kame::love2d::detail::physics::PolygonShape::getPoints() const
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    std::vector<float> points(polygonShape.m_count * 2);
    points.clear();

    for (int i = 0; i < polygonShape.m_count; ++i)
    {
        b2Vec2 v = polygonShape.m_vertices[i];
        points.emplace_back(ctx.physics->scaleUp(v.x));
        points.emplace_back(ctx.physics->scaleUp(v.y));
    }

    return points;
}

const kame::love2d::Shape* kame::love2d::detail::physics::Fixture::getShape()
{
    b2Shape* s = fixture->GetShape();
    assert(s->GetType() == b2Shape::Type::e_circle || s->GetType() == b2Shape::Type::e_polygon);

    if (s->GetType() == b2Shape::Type::e_polygon)
    {
        kame::love2d::detail::physics::PolygonShape poly = kame::love2d::detail::physics::PolygonShape();

        memcpy((void*)&poly.polygonShape, (void*)s, sizeof(b2PolygonShape));

        shape = poly;

        return &std::get<kame::love2d::detail::physics::PolygonShape>(shape);
    }
    else
    {
        kame::love2d::detail::physics::CircleShape circle = kame::love2d::detail::physics::CircleShape();

        memcpy((void*)&circle.circleShape, (void*)s, sizeof(b2CircleShape));

        shape = circle;

        return &std::get<kame::love2d::detail::physics::CircleShape>(shape);
    }
}

std::pair<float, float> kame::love2d::detail::physics::Contact::getNormal() const
{
    b2WorldManifold manifold;
    contact->GetWorldManifold(&manifold);
    return {manifold.normal.x, manifold.normal.y};
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

void kame::love2d::detail::physics::Physics::destroyQueues()
{
    auto& ctx = kame::love2d::detail::Context::getInstance();

    for (auto* fixture : destroyQueueFixture)
    {
        if (ctx.isValid())
        {
            assert(ctx.physics->fixtureMap.contains(fixture));
            ctx.physics->fixtureMap.erase(ctx.physics->fixtureMap.find(fixture));
        }
        fixture->GetBody()->DestroyFixture(fixture);
        kame::love2d::detail::physics::Physics::deletedFixtureCount++;
    }
    destroyQueueFixture.clear();

    for (auto* body : destroyQueueBody)
    {
        body->GetWorld()->DestroyBody(body);
        kame::love2d::detail::physics::Physics::deletedBodyCount++;
    }
    destroyQueueBody.clear();

    for (auto* world : destroyQueueWorld)
    {
        delete world;
        kame::love2d::detail::physics::Physics::deletedWorldCount++;
    }
    destroyQueueWorld.clear();
}

std::vector<b2Fixture*> kame::love2d::detail::physics::Physics::destroyQueueFixture;
std::vector<b2Body*> kame::love2d::detail::physics::Physics::destroyQueueBody;
std::vector<b2World*> kame::love2d::detail::physics::Physics::destroyQueueWorld;

size_t kame::love2d::detail::physics::Physics::createdWorldCount = 0;
size_t kame::love2d::detail::physics::Physics::deletedWorldCount = 0;
size_t kame::love2d::detail::physics::Physics::createdBodyCount = 0;
size_t kame::love2d::detail::physics::Physics::deletedBodyCount = 0;
size_t kame::love2d::detail::physics::Physics::createdFixtureCount = 0;
size_t kame::love2d::detail::physics::Physics::deletedFixtureCount = 0;
kame::love2d::detail::physics::DebugDraw kame::love2d::detail::physics::Physics::debugDraw;

kame::love2d::detail::physics::World* kame::love2d::detail::physics::Physics::newWorld(float xg, float yg, bool sleep)
{
    kame::love2d::detail::physics::World* world = new kame::love2d::detail::physics::World();
    assert(world);

    world->world = new b2World(scaleDown(b2Vec2(xg, yg)));
    assert(world->world);
    world->world->SetAllowSleeping(sleep);

    world->world->SetDebugDraw(&kame::love2d::detail::physics::Physics::debugDraw);
    // kame::love2d::detail::physics::Physics::debugDraw.SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);
    kame::love2d::detail::physics::Physics::debugDraw.SetFlags(b2Draw::e_shapeBit);

    kame::love2d::detail::physics::Physics::createdWorldCount++;

    return world;
}

kame::love2d::detail::physics::Body* kame::love2d::detail::physics::Physics::newBody(kame::love2d::detail::physics::World* world, float x, float y, const char* type)
{
    assert(type);
    assert(std::string(type) == "kinematic" || std::string(type) == "dynamic" || std::string(type) == "static");

    b2BodyDef defBody = b2BodyDef();
    if (std::string(type) == "dynamic")
    {
        defBody.type = b2_dynamicBody;
    }
    else if (std::string(type) == "kinematic")
    {
        defBody.type = b2_kinematicBody;
    }
    defBody.position = scaleDown(b2Vec2(x, y));

    kame::love2d::detail::physics::Body* body = new kame::love2d::detail::physics::Body();
    assert(body);

    body->body = world->world->CreateBody(&defBody);
    assert(body->body);

    kame::love2d::detail::physics::Physics::createdBodyCount++;

    return body;
}

kame::love2d::detail::physics::CircleShape kame::love2d::detail::physics::Physics::newCircleShape(float radius)
{
    kame::love2d::detail::physics::CircleShape shape;
    shape.circleShape = b2CircleShape();

    shape.circleShape.m_p.SetZero();
    shape.circleShape.m_radius = scaleDown(radius);

    return shape;
}

kame::love2d::detail::physics::PolygonShape kame::love2d::detail::physics::Physics::newPolygonShape(std::vector<float>& vertices)
{
    assert(vertices.size() % 2 == 0);
    size_t vcount = vertices.size() / 2;
    assert(vcount >= 3);
    assert(vcount <= b2_maxPolygonVertices);

    kame::love2d::detail::physics::PolygonShape shape;
    shape.polygonShape = b2PolygonShape();

    std::vector<float> points = vertices;
    for (auto& i : points)
    {
        i = scaleDown(i);
    }

    shape.polygonShape.Set((b2Vec2*)points.data(), vcount);

    return shape;
}

kame::love2d::detail::physics::PolygonShape kame::love2d::detail::physics::Physics::newRectangleShape(float width, float height)
{
    kame::love2d::detail::physics::PolygonShape shape;
    shape.polygonShape = b2PolygonShape();

    shape.polygonShape.SetAsBox(scaleDown(width / 2.0f), scaleDown(height / 2.0f));

    return shape;
}

kame::love2d::detail::physics::PolygonShape kame::love2d::detail::physics::Physics::newRectangleShape(float x, float y, float width, float height, float angle)
{
    kame::love2d::detail::physics::PolygonShape shape;
    shape.polygonShape = b2PolygonShape();

    shape.polygonShape.SetAsBox(scaleDown(width / 2.0f), scaleDown(height / 2.0f), scaleDown(b2Vec2(x, y)), angle);

    return shape;
}

kame::love2d::detail::physics::Fixture* kame::love2d::detail::physics::Physics::newFixture(Body* body, const CircleShape& shape, float density)
{
    kame::love2d::detail::physics::Fixture* fixture = new kame::love2d::detail::physics::Fixture();
    assert(fixture);

    b2FixtureDef defFixture = b2FixtureDef();

    defFixture.shape = &shape.circleShape;
    defFixture.density = density;

    fixture->fixture = body->body->CreateFixture(&defFixture);
    assert(fixture->fixture);

    kame::love2d::detail::physics::Physics::createdFixtureCount++;

    return fixture;
}

kame::love2d::detail::physics::Fixture* kame::love2d::detail::physics::Physics::newFixture(Body* body, const PolygonShape& shape, float density)
{
    kame::love2d::detail::physics::Fixture* fixture = new kame::love2d::detail::physics::Fixture();
    assert(fixture);

    b2FixtureDef defFixture = b2FixtureDef();

    defFixture.shape = &shape.polygonShape;
    defFixture.density = density;

    fixture->fixture = body->body->CreateFixture(&defFixture);
    assert(fixture->fixture);

    kame::love2d::detail::physics::Physics::createdFixtureCount++;

    return fixture;
}

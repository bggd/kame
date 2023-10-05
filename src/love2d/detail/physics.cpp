#include <all.hpp>

kame::love2d::detail::physics::World::~World()
{
    release();
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
        ctx.physics->destroyQueueWorld.emplace_back(world);
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
    release();
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
        ctx.physics->destroyQueueBody.emplace_back(body);
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
    release();
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
        ctx.physics->destroyQueueFixture.emplace_back(fixture);
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

    ctx.physics->destroyQueues();
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
    for (auto* fixture : destroyQueueFixture)
    {
        fixture->GetBody()->DestroyFixture(fixture);
    }
    destroyQueueFixture.clear();

    for (auto* body : destroyQueueBody)
    {
        body->GetWorld()->DestroyBody(body);
    }
    destroyQueueBody.clear();

    for (auto* world : destroyQueueWorld)
    {
        delete world;
    }
    destroyQueueWorld.clear();
}

kame::love2d::detail::physics::World* kame::love2d::detail::physics::Physics::newWorld(float xg, float yg, bool sleep)
{
    kame::love2d::detail::physics::World* world = new kame::love2d::detail::physics::World();
    assert(world);

    world->world = new b2World(scaleDown(b2Vec2(xg, yg)));
    assert(world->world);
    world->world->SetAllowSleeping(sleep);

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

    return fixture;
}

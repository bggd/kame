#include <all.hpp>

std::unordered_map<b2Fixture*, kame::love2d::detail::box2d::SPtrBox2dFixture::weak_type> kame::love2d::detail::box2d::fixtureMap;

namespace kame::love2d::detail::box2d {

const std::vector<float>& Box2dPolygonShape::getPoints()
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    _points.reserve(_polygonShapeB2D.m_count * 2);
    _points.clear();
    for (int i = 0; i < _polygonShapeB2D.m_count; ++i)
    {
        _points.emplace_back(ctx.physics->scaleUp(_polygonShapeB2D.m_vertices[i].x));
        _points.emplace_back(ctx.physics->scaleUp(_polygonShapeB2D.m_vertices[i].y));
    }
    return _points;
}

Box2dWorld::~Box2dWorld()
{
    assert(_worldB2D);
    assert(!_worldB2D->IsLocked());
    assert(_worldB2D->GetBodyCount() == 0);
    _worldB2D->SetDebugDraw(nullptr);
    _worldB2D->SetContactListener(nullptr);
    _worldB2D->SetDestructionListener(nullptr);
    if (_listener.has_value())
    {
        auto* p = std::any_cast<kame::love2d::detail::physics::ContactListener*>(_listener);
        delete p;
        _listener = nullptr;
    }
    delete _worldB2D;
    _worldB2D = nullptr;
}

Box2dBody::~Box2dBody()
{
    assert(_bodyB2D);
    assert(!_parentWorld->_worldB2D->IsLocked());
    assert(_bodyB2D->GetFixtureList() == nullptr);
    _bodyB2D->GetWorld()->DestroyBody(_bodyB2D);
    _bodyB2D = nullptr;
}

Box2dFixture::~Box2dFixture()
{
    assert(_fixtureB2D);
    assert(!_parentBody->_bodyB2D->GetWorld()->IsLocked());
    assert(fixtureMap.contains(_fixtureB2D));
    assert(fixtureMap[_fixtureB2D].expired());
    // FIXME: write more safe code
    {
        // legal?
        auto sptr = SPtrBox2dFixture(this, [](SPtrBox2dFixture::element_type*) {});
        fixtureMap[_fixtureB2D] = sptr;
        _parentBody->_bodyB2D->DestroyFixture(_fixtureB2D);
    }
    assert(fixtureMap[_fixtureB2D].expired());
    fixtureMap.erase(fixtureMap.find(_fixtureB2D));
    _fixtureB2D = nullptr;
}
float Box2dBody::getX()
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return ctx.physics->scaleUp(_bodyB2D->GetPosition().x);
}

float Box2dBody::getY()
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return ctx.physics->scaleUp(_bodyB2D->GetPosition().y);
}

std::vector<float> Box2dBody::getWorldPoints(std::vector<float>& points)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    std::vector<float> vertices(points);
    for (size_t i = 0; i < points.size() * 2; i += 2)
    {
        b2Vec2 v;
        v.x = points[i];
        v.y = points[i + 1];
        v = ctx.physics->scaleDown(v);
        v = _bodyB2D->GetWorldPoint(v);
        v = ctx.physics->scaleUp(v);
        vertices[i] = v.x;
        vertices[i + 1] = v.y;
    }

    return vertices;
}

struct Box2dDestructionListener : b2DestructionListener {
    void SayGoodbye(b2Joint*) { assert(0); }
    void SayGoodbye(b2Fixture*) { assert(0); }
};
static Box2dDestructionListener destructionListener;

static kame::love2d::detail::box2d::DebugDraw debugDraw;

SPtrBox2dWorld newSPtrBox2dWorld(b2Vec2 gravity, bool sleep)
{
    auto* w = new Box2dWorld();
    assert(w);

    w->_worldB2D = new b2World(gravity);
    assert(w->_worldB2D);
    w->_worldB2D->SetAllowSleeping(sleep);

    w->_worldB2D->SetDestructionListener(&destructionListener);

    debugDraw.SetFlags(b2Draw::e_shapeBit);
    w->_worldB2D->SetDebugDraw(&debugDraw);

    return SPtrBox2dWorld(w);
}

SPtrBox2dBody newSPtrBox2dBody(SPtrBox2dWorld w, float x, float y, b2BodyType type)
{
    auto* b = new Box2dBody();
    assert(b);

    b2BodyDef def = b2BodyDef();
    def.type = type;
    def.position.Set(x, y);
    b->_bodyB2D = w->_worldB2D->CreateBody(&def);
    assert(b->_bodyB2D);

    b->_parentWorld = w;

    return SPtrBox2dBody(b);
}

SPtrBox2dFixture newSPtrBox2DFixture(SPtrBox2dBody body, const Box2dShape& shape, float density)
{
    auto* f = new Box2dFixture();
    assert(f);

    b2FixtureDef def = b2FixtureDef();
    def.density = density;

    if (std::holds_alternative<Box2dCircleShape>(shape))
    {
        auto& s = std::get<Box2dCircleShape>(shape);
        def.shape = &s._circleShapeB2D;
    }
    else
    {
        auto& s = std::get<Box2dPolygonShape>(shape);
        def.shape = &s._polygonShapeB2D;
    }

    f->_fixtureB2D = body->_bodyB2D->CreateFixture(&def);
    assert(f->_fixtureB2D);

    f->_parentBody = body;

    auto sptr = SPtrBox2dFixture(f);

    fixtureMap[f->_fixtureB2D] = sptr;

    return sptr;
}

void Box2dWorld::update(float dt, int velocityiterations, int positioniterations)
{
    _worldB2D->Step(dt, velocityiterations, positioniterations);
}

void Box2dWorld::debugDraw()
{
    _worldB2D->DebugDraw();
}

} // namespace kame::love2d::detail::box2d

#include <all.hpp>

std::unordered_map<b2Fixture*, kame::love2d::detail::box2d::SPtrBox2dFixture::weak_type> kame::love2d::detail::box2d::fixtureMap;

kame::love2d::detail::box2d::Box2dWorld::~Box2dWorld()
{
    assert(_worldB2D);
    assert(!_worldB2D->IsLocked());
    assert(_worldB2D->GetBodyCount() == 0);
    _worldB2D->SetContactListener(nullptr);
    _worldB2D->SetDestructionListener(nullptr);
    delete _worldB2D;
    _worldB2D = nullptr;
}

kame::love2d::detail::box2d::Box2dBody::~Box2dBody()
{
    assert(_bodyB2D);
    assert(!parentWorld->_worldB2D->IsLocked());
    assert(_bodyB2D->GetFixtureList() == nullptr);
    _bodyB2D->GetWorld()->DestroyBody(_bodyB2D);
    _bodyB2D = nullptr;
}

float kame::love2d::detail::box2d::Box2dBody::getX()
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return ctx.physics->scaleUp(_bodyB2D->GetPosition().x);
}

float kame::love2d::detail::box2d::Box2dBody::getY()
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return ctx.physics->scaleUp(_bodyB2D->GetPosition().y);
}

kame::love2d::detail::box2d::Box2dFixture::~Box2dFixture()
{
    assert(_fixtureB2D);
    assert(!parentBody->_bodyB2D->GetWorld()->IsLocked());
    assert(fixtureMap.contains(_fixtureB2D));
    assert(fixtureMap[_fixtureB2D].expired());
    // FIXME: write more safe code
    {
        // legal?
        auto sptr = SPtrBox2dFixture(this, [](SPtrBox2dFixture::element_type*) {});
        fixtureMap[_fixtureB2D] = sptr;
        parentBody->_bodyB2D->DestroyFixture(_fixtureB2D);
    }
    assert(fixtureMap[_fixtureB2D].expired());
    fixtureMap.erase(fixtureMap.find(_fixtureB2D));
    _fixtureB2D = nullptr;
}

struct Box2dDestructionListener : b2DestructionListener {
    void SayGoodbye(b2Joint*) { assert(0); }
    void SayGoodbye(b2Fixture*) { assert(0); }
};
static Box2dDestructionListener destructionListener;

kame::love2d::detail::box2d::SPtrBox2dWorld kame::love2d::detail::box2d::newSPtrBox2dWorld(b2Vec2 gravity, bool sleep)
{
    auto* w = new kame::love2d::detail::box2d::Box2dWorld();
    assert(w);

    w->_worldB2D = new b2World(gravity);
    assert(w->_worldB2D);
    w->_worldB2D->SetAllowSleeping(sleep);

    w->_worldB2D->SetDestructionListener(&destructionListener);

    return kame::love2d::detail::box2d::SPtrBox2dWorld(w);
}

kame::love2d::detail::box2d::SPtrBox2dBody kame::love2d::detail::box2d::newSPtrBox2dBody(kame::love2d::detail::box2d::SPtrBox2dWorld w, float x, float y, b2BodyType type)
{
    auto* b = new kame::love2d::detail::box2d::Box2dBody();
    assert(b);

    b2BodyDef def = b2BodyDef();
    def.type = type;
    def.position.Set(x, y);
    b->_bodyB2D = w->_worldB2D->CreateBody(&def);
    assert(b->_bodyB2D);

    b->parentWorld = w;

    return kame::love2d::detail::box2d::SPtrBox2dBody(b);
}

kame::love2d::detail::box2d::SPtrBox2dFixture kame::love2d::detail::box2d::newSPtrBox2DFixture(kame::love2d::detail::box2d::SPtrBox2dBody body, const kame::love2d::detail::box2d::Box2dShape& shape, float density)
{
    auto* f = new kame::love2d::detail::box2d::Box2dFixture();
    assert(f);

    b2FixtureDef def = b2FixtureDef();
    def.density = density;

    if (std::holds_alternative<kame::love2d::detail::box2d::Box2dCircleShape>(shape))
    {
        auto& s = std::get<kame::love2d::detail::box2d::Box2dCircleShape>(shape);
        def.shape = &s._circleShapeB2D;
    }
    else
    {
        auto& s = std::get<kame::love2d::detail::box2d::Box2dPolygonShape>(shape);
        def.shape = &s._polygonShapeB2D;
    }

    f->_fixtureB2D = body->_bodyB2D->CreateFixture(&def);
    assert(f->_fixtureB2D);

    f->parentBody = body;

    auto sptr = kame::love2d::detail::box2d::SPtrBox2dFixture(f);

    fixtureMap[f->_fixtureB2D] = sptr;

    return sptr;
}

void kame::love2d::detail::box2d::Box2dWorld::update(float dt, int velocityiterations, int positioniterations)
{
    _worldB2D->Step(dt, velocityiterations, positioniterations);
}

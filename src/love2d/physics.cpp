#include <all.hpp>

void kame::love2d::physics::setMeter(float scale)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    ctx.physics->setMeter(scale);
}

float kame::love2d::physics::getMeter()
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return ctx.physics->getMeter();
}

kame::love2d::World kame::love2d::physics::newWorld(float xg, float yg, bool sleep)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return kame::love2d::World(ctx.physics->newWorld(xg, yg, sleep));
}

kame::love2d::Body kame::love2d::physics::newBody(kame::love2d::World world, float x, float y, const char* type)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return kame::love2d::Body(ctx.physics->newBody(world.get(), x, y, type));
}

kame::love2d::PolygonShape kame::love2d::physics::newPolygonShape(std::vector<float>& vertices)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return kame::love2d::PolygonShape(ctx.physics->newPolygonShape(vertices));
}

kame::love2d::PolygonShape kame::love2d::physics::newPolygonShape(std::vector<float> vertices)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return kame::love2d::PolygonShape(ctx.physics->newPolygonShape(vertices));
}

kame::love2d::PolygonShape kame::love2d::physics::newRectangleShape(float width, float height)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return kame::love2d::PolygonShape(ctx.physics->newRectangleShape(width, height));
}

kame::love2d::PolygonShape kame::love2d::physics::newRectangleShape(float x, float y, float width, float height, float angle)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return kame::love2d::PolygonShape(ctx.physics->newRectangleShape(x, y, width, height, angle));
}

kame::love2d::Fixture kame::love2d::physics::newFixture(kame::love2d::Body body, kame::love2d::PolygonShape shape, float density)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return kame::love2d::Fixture(ctx.physics->newFixture(body.get(), shape.get(), density));
}

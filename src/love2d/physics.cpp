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
    return ctx.physics->newWorld(xg, yg, sleep);
}

kame::love2d::Body kame::love2d::physics::newBody(kame::love2d::World world, float x, float y, const char* type)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return ctx.physics->newBody(world->_world, x, y, type);
}

kame::love2d::CircleShape kame::love2d::physics::newCircleShape(float radius)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return ctx.physics->newCircleShape(radius);
}

kame::love2d::PolygonShape kame::love2d::physics::newPolygonShape(std::vector<float>& vertices)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return ctx.physics->newPolygonShape(vertices);
}

kame::love2d::PolygonShape kame::love2d::physics::newRectangleShape(float width, float height)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return ctx.physics->newRectangleShape(width, height);
}

kame::love2d::PolygonShape kame::love2d::physics::newRectangleShape(float x, float y, float width, float height, float angle)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return ctx.physics->newRectangleShape(x, y, width, height, angle);
}

kame::love2d::Fixture kame::love2d::physics::newFixture(kame::love2d::Body body, const kame::love2d::CircleShape& shape, float density)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return ctx.physics->newFixture(body, shape, density);
}

kame::love2d::Fixture kame::love2d::physics::newFixture(kame::love2d::Body body, const kame::love2d::PolygonShape& shape, float density)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());
    return ctx.physics->newFixture(body, shape, density);
}

kame::love2d::Fixture kame::love2d::physics::newFixture(Body body, const Shape& shape, float density)
{
    auto& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    assert(shape.index() > 0);

    if (std::holds_alternative<kame::love2d::CircleShape>(shape))
    {
        return kame::love2d::physics::newFixture(body, std::get<kame::love2d::CircleShape>(shape), density);
    }
    else
    {
        return kame::love2d::physics::newFixture(body, std::get<kame::love2d::PolygonShape>(shape), density);
    }
}

#include <all.hpp>

struct DebugDraw : b2Draw {
    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
    void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;
    void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;
    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
    void DrawTransform(const b2Transform& xf) override;
    void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;
};

void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    std::vector<float> vecs(vertexCount * 2);
    vecs.clear();
    for (int32 i = 0; i < vertexCount; ++i)
    {
        vecs.emplace_back(ctx.physics->scaleUp(vertices[i].x));
        vecs.emplace_back(ctx.physics->scaleUp(vertices[i].y));
    }

    kame::love2d::graphics::setColor(color.r, color.g, color.b, color.a);
    kame::love2d::graphics::polygon("line", vecs);
}

void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    std::vector<float> vecs(vertexCount * 2);
    vecs.clear();
    for (int32 i = 0; i < vertexCount; ++i)
    {
        vecs.emplace_back(ctx.physics->scaleUp(vertices[i].x));
        vecs.emplace_back(ctx.physics->scaleUp(vertices[i].y));
    }

    b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
    kame::love2d::graphics::setColor(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    kame::love2d::graphics::polygon("fill", vecs);

    kame::love2d::graphics::setColor(color.r, color.g, color.b, color.a);
    kame::love2d::graphics::polygon("line", vecs);
}

void DebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    radius = ctx.physics->scaleUp(radius);
    b2Vec2 c = ctx.physics->scaleUp(center);

    kame::love2d::graphics::setColor(color.r, color.g, color.b, color.a);
    kame::love2d::graphics::circle("line", c.x, c.y, radius, 8);
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    b2Vec2 c = ctx.physics->scaleUp(center);

    b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
    kame::love2d::graphics::setColor(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    kame::love2d::graphics::circle("fill", c.x, c.y, ctx.physics->scaleUp(radius), 8);

    kame::love2d::graphics::setColor(color.r, color.g, color.b, color.a);
    kame::love2d::graphics::circle("line", c.x, c.y, ctx.physics->scaleUp(radius), 8);

    b2Vec2 p = ctx.physics->scaleUp(center + radius * axis);
    kame::love2d::graphics::line(c.x, c.y, p.x, p.y);
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    b2Vec2 v1 = ctx.physics->scaleUp(p1);
    b2Vec2 v2 = ctx.physics->scaleUp(p2);

    kame::love2d::graphics::setColor(color.r, color.g, color.b, color.a);
    kame::love2d::graphics::line(v1.x, v1.y, v2.x, v2.y);
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{
    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    const float k_axisScale = 0.4f;
    b2Color red(1.0f, 0.0f, 0.0f);
    b2Color green(0.0f, 1.0f, 0.0f);
    b2Vec2 p1 = xf.p, p2;

    p2 = p1 + k_axisScale * xf.q.GetXAxis();
    b2Vec2 v1 = ctx.physics->scaleUp(p1);
    b2Vec2 v2 = ctx.physics->scaleUp(p2);
    kame::love2d::graphics::setColor(red.r, red.g, red.b, red.a);
    kame::love2d::graphics::line(v1.x, v1.y, v2.x, v2.y);

    p2 = p1 + k_axisScale * xf.q.GetYAxis();
    kame::love2d::graphics::setColor(green.r, green.g, green.b, green.a);
    kame::love2d::graphics::line(v1.x, v1.y, v2.x, v2.y);
}

void DebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
    (void)(size);

    kame::love2d::detail::Context& ctx = kame::love2d::detail::Context::getInstance();
    assert(ctx.isValid());

    b2Vec2 v = ctx.physics->scaleUp(p);

    kame::love2d::graphics::setColor(color.r, color.g, color.b, color.a);
    kame::love2d::graphics::points(v.x, v.y);
}

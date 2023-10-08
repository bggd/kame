#pragma once

#include "detail/graphics.hpp"
#include "detail/physics.hpp"

#include <memory>

namespace kame::love2d {

using Image = std::shared_ptr<kame::love2d::detail::graphics::Image>;
using Quad = std::shared_ptr<kame::love2d::detail::graphics::Quad>;

using Shape = kame::love2d::detail::box2d::Box2dShape;
using CircleShape = kame::love2d::detail::box2d::Box2dCircleShape;
using PolygonShape = kame::love2d::detail::box2d::Box2dPolygonShape;

using Contact = kame::love2d::detail::box2d::Box2dContact;

using World = kame::love2d::detail::physics::SPtrWorld;
using Body = kame::love2d::detail::box2d::SPtrBox2dBody;
using Fixture = kame::love2d::detail::box2d::SPtrBox2dFixture;

using CollisionCallbackContact = std::function<void(kame::love2d::Fixture, kame::love2d::Fixture, const kame::love2d::Contact*)>;
using CollisionCallbackContactPostResolve = std::function<void(kame::love2d::Fixture, kame::love2d::Fixture, const kame::love2d::Contact*, const std::vector<std::pair<float, float>>&)>;

} // namespace kame::love2d

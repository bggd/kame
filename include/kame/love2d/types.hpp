#pragma once

#include "detail/graphics.hpp"
#include "detail/physics.hpp"

#include <memory>

namespace kame::love2d {

using Image = std::shared_ptr<kame::love2d::detail::graphics::Image>;
using Quad = std::shared_ptr<kame::love2d::detail::graphics::Quad>;

using World = std::shared_ptr<kame::love2d::detail::physics::World>;
using Body = std::shared_ptr<kame::love2d::detail::physics::Body>;
using Shape = kame::love2d::detail::physics::Shape;
using CircleShape = kame::love2d::detail::physics::CircleShape;
using PolygonShape = kame::love2d::detail::physics::PolygonShape;
using Fixture = std::shared_ptr<kame::love2d::detail::physics::Fixture>;

} // namespace kame::love2d

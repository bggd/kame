#pragma once

#include "detail/graphics.hpp"

#include <memory>

namespace kame::love2d {

using Image = std::shared_ptr<kame::love2d::detail::graphics::Image>;
using Quad = std::shared_ptr<kame::love2d::detail::graphics::Quad>;

} // namespace kame::love2d

#pragma once

#include <memory>

#include "detail/graphics.hpp"

namespace kame::love2d::graphics {

using Image = std::shared_ptr<kame::love2d::detail::graphics::Image>;
using Quad = std::shared_ptr<kame::love2d::detail::graphics::Quad>;

Image newImage(const char* filename);
Quad newQuad(int x, int y, int width, int height, int sw, int sh);

void draw(Image drawable, float x, float y, float r, float sx, float sy, float ox = 0.0f, float oy = 0.0f);

void draw(Image drawable, float x = 0.0f, float y = 0.0f, float r = 0.0f, float sx = 1.0f);

void draw(Image drawable, Quad quad, float x, float y, float r, float sx, float sy, float ox = 0.0f, float oy = 0.0f);

void draw(Image drawable, Quad quad, float x = 0.0f, float y = 0.0f, float r = 0.0f, float sx = 1.0f);


} // namespace kame::love2d::graphics

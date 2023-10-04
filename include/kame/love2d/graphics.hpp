#pragma once

#include <initializer_list>

#include "types.hpp"

namespace kame::love2d::graphics {

Image newImage(const char* filename);

Quad newQuad(int x, int y, int width, int height, int sw, int sh);

void circle(const char* mode, float x, float y, float radius, int segments);

void draw(Image drawable, float x, float y, float r, float sx, float sy, float ox = 0.0f, float oy = 0.0f);

void draw(Image drawable, float x = 0.0f, float y = 0.0f, float r = 0.0f, float sx = 1.0f);

void draw(Image drawable, Quad quad, float x, float y, float r, float sx, float sy, float ox = 0.0f, float oy = 0.0f);

void draw(Image drawable, Quad quad, float x = 0.0f, float y = 0.0f, float r = 0.0f, float sx = 1.0f);

void polygon(const char* mode, std::vector<float>& vertices);
void polygon(const char* mode, std::vector<float> vertices);

template <typename... Args>
void polygon(const char* mode, Args... args)
{
    polygon(mode, {args...});
}

} // namespace kame::love2d::graphics

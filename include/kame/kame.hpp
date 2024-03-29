#pragma once

namespace kame {

void kameInit();
void kameShutdown();
bool kameIsInit();

} // namespace kame

#include "sdl/sdl.hpp"
#include "ogl/ogl.hpp"
#include "vk/vk.hpp"
#include "math/math.hpp"
#include "gltf/gltf.hpp"
#include "squirtle/squirtle.hpp"

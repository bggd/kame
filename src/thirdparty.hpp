#pragma once

#if __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds="
#pragma GCC diagnostic ignored "-Wstringop-overflow="
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough="
#endif
#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include <spdlog/spdlog.h>
#if __GCC__
#pragma GCC disgnostic pop
#endif
#if __clang__
#pragma clang diagnostic pop
#endif

#include <stb_image.h>

#include <pystring.h>

#include <magic_enum.hpp>

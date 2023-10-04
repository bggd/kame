#pragma once
#include <kame/kame.hpp>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include <spdlog/spdlog.h>

#include "stb_image.h"

#include <pystring.h>

#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <array>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>
#include <fstream>
#include <tuple>
#include <span>

// assert is enabled on release build
#ifdef NDEBUG
#undef NDEBUG
#include <cassert>
#define NDEBUG
#else
#include <cassert>
#endif

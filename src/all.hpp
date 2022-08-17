#pragma once

#include <kame/kame.hpp>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include <spdlog/spdlog.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"

#include "../pystring/pystring.h"

#include <cassert>
#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <string>
#include <vector>
#include <stack>
#include <unordered_set>
#include <fstream>
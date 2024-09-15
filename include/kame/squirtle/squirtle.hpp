#pragma once


#include "model.hpp"

#include "engine_ogl.hpp"

namespace kame::squirtle {

char* loadFile(const char* fileName, int64_t& len);
char* loadFile(const char* fileName);

std::vector<char> loadSPIRV(const char* fileName);

} // namespace kame::squirtle

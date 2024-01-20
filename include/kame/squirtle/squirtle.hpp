#include "model.hpp"

namespace kame::squirtle {

char* loadFile(const char* fileName, int64_t& len);
char* loadFile(const char* fileName);

std::vector<char> loadSPIRV(const char* fileName);

} // namespace kame::squirtle

#include <all.hpp>

namespace kame {

void kameInit()
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("kameInit");
}

void kameShutdown()
{
    spdlog::info("kameShutdown");
}

} // namespace kame
#pragma once
#include <kame/kame.hpp>

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

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <array>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>
#include <fstream>
#include <tuple>
#include <span>
#include <filesystem>
#include <functional>
#include <source_location>

// assert is enabled on release build
#ifdef NDEBUG
#undef NDEBUG
#include <cassert>
#define NDEBUG
#else
#include <cassert>
#endif

[[maybe_unused]] static bool checkVulkanFunction(const char* fnName, VkResult result, std::source_location loc = std::source_location::current())
{
    if (result != VK_SUCCESS)
    {
        SPDLOG_CRITICAL("[Vulkan] Fatal on {} at {}:{}: {}", fnName, loc.file_name(), loc.line(), result);
    }
    return result == VK_SUCCESS;
}

[[maybe_unused]] static bool checkVulkanFunctionImcomplete(const char* fnName, VkResult result, std::source_location loc = std::source_location::current())
{
    if (result != VK_SUCCESS && result != VK_INCOMPLETE)
    {
        SPDLOG_CRITICAL("[Vulkan] Fatal on {} at {}:{}: {}", fnName, loc.file_name(), loc.line(), result);
    }
    if (result == VK_INCOMPLETE)
    {
        SPDLOG_DEBUG("[Vulkan] VK_INCOMPLETE is returned on {} at {}:{}", fnName, loc.file_name(), loc.line());
    }

    return result == VK_SUCCESS || result == VK_INCOMPLETE;
}

// clang-format off
#define VK_CHECK(call) do { bool b = checkVulkanFunction(#call, call); assert(b); } while (0)

#define VK_CHECK_INCOMPLETE(call)  do { bool b = checkVulkanFunctionImcomplete(#call, call); assert(b); } while (0)
// clang-format on

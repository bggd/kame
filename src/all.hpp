#pragma once

#include "std.hpp"

#include "thirdparty.hpp"

#include <kame/kame.hpp>

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
        auto n = magic_enum::enum_name(result);

        SPDLOG_CRITICAL("[Vulkan] Fatal on {} at {}:{}: {}", fnName, loc.file_name(), loc.line(), n);
    }

    return result == VK_SUCCESS;
}

[[maybe_unused]] static bool checkVulkanFunctionImcomplete(const char* fnName, VkResult result, std::source_location loc = std::source_location::current())
{
    if (result != VK_SUCCESS && result != VK_INCOMPLETE)
    {
        auto n = magic_enum::enum_name(result);

        SPDLOG_CRITICAL("[Vulkan] Fatal on {} at {}:{}: {}", fnName, loc.file_name(), loc.line(), n);
    }
    if (result == VK_INCOMPLETE)
    {
        SPDLOG_WARN("[Vulkan] VK_INCOMPLETE is returned on {} at {}:{}", fnName, loc.file_name(), loc.line());
    }

    return result == VK_SUCCESS || result == VK_INCOMPLETE;
}

// clang-format off
#define VK_CHECK(call) do { bool b = checkVulkanFunction(#call, call); assert(b); } while (0)

#define VK_CHECK_INCOMPLETE(call)  do { bool b = checkVulkanFunctionImcomplete(#call, call); assert(b); } while (0)
// clang-format on

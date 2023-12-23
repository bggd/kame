#pragma once

#include "vk_header.hpp"

#include <utility>
#include <vector>
#include <string>

#include <kame/sdl/sdl.hpp>

namespace kame::vk {

struct Vulkan {

    std::vector<VkExtensionProperties> _extensionProperties;

    std::vector<const char*> _extensions;

    std::vector<VkLayerProperties> _layerProperties;

    std::vector<const char*> _validationLayers;

    VkDebugUtilsMessengerCreateInfoEXT _debugInfo;
    VkDebugUtilsMessengerEXT _debugMessanger = VK_NULL_HANDLE;

    VkInstance _instance = VK_NULL_HANDLE;

    bool _isInitialized = false;

    bool _hasDebugUtils = false;
    bool _hasKHR_PORTABILITY_ENUMERATION = false;

    // validation layers
    bool _hasKHRONOS_validation = false;
    bool _hasKHRONOS_profiles = false;

    void initExtensions();

    void initValidationLayers();

    void createInstance(kame::sdl::WindowVk& window);

    void startup(kame::sdl::WindowVk& window);

    void destroyInstance();

    void shutdown();
};

} // namespace kame::vk

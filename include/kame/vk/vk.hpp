#pragma once

#include "vk_header.hpp"

#include <utility>
#include <vector>
#include <string>

#include <kame/sdl/sdl.hpp>

#define KAME_VK_MAX_FRAMES_IN_FLIGHT 2

namespace kame::vk {

struct Vulkan {

    std::vector<VkExtensionProperties> _extensionProperties;

    std::vector<const char*> _extensions;

    std::vector<VkLayerProperties> _layerProperties;

    std::vector<const char*> _validationLayers;

    VkDebugUtilsMessengerCreateInfoEXT _debugInfo;
    VkDebugUtilsMessengerEXT _debugMessanger = VK_NULL_HANDLE;

    VkInstance _instance = VK_NULL_HANDLE;

    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceMemoryProperties _memProperties;
    VkDevice _device = VK_NULL_HANDLE;

    uint32_t _qFamilyGraphicsIndex = 0;
    VkQueue _graphicsQueue = VK_NULL_HANDLE;

    VkCommandPool _commandPool = VK_NULL_HANDLE;

    std::vector<VkCommandBuffer> _cmdBuffers;
    std::vector<VkFence> _inFlightFences;

    uint32_t currentFrame = 0;

    bool _isInitialized = false;

    bool _hasDebugUtils = false;
    bool _hasKHR_PORTABILITY_ENUMERATION = false;
    bool _hasKHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION = false;
    // bool _hasKHR_portability_subset = false;

    // validation layers
    bool _hasKHRONOS_validation = false;
    bool _hasKHRONOS_profiles = false;

    void initLoader();
    void deinitLoader();

    void initExtensions();

    void initValidationLayers();

    void createInstance(const char* appName = "");

    void pickPhysicalDevice();

    void initMemProperties();

    void createDevice();

    void createQueue();

    void createCommandPool();

    void createCommandBuffers();

    void createSyncObjects();

    void startupHeadless(const char* appName = "");

    void destroyInstance();

    void destroyDevice();

    void destroyQueue();

    void destroyCommandPool();

    void destroyCommandBuffers();

    void destroySyncObjects();

    void shutdown();

    bool _findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, uint32_t& i, uint32_t& type);

    VkResult allocateMemory(const VkMemoryRequirements& memRequirements, VkMemoryPropertyFlags properties, VkDeviceMemory& deviceMemory);

    void freeMemory(VkDeviceMemory& mem);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VkMemoryRequirements& memRequirements);

    void destroyBuffer(VkBuffer& buffer);

    void bindBufferMemory(VkBuffer buffer, VkDeviceMemory deviceMemory, VkDeviceSize memoryOffset = 0);

    void createShaderModule(const std::vector<char>&, VkShaderModule& shader);

    void destroyShaderModule(VkShaderModule& shader);
};

} // namespace kame::vk

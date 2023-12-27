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

    uint32_t _numFramesInFlight = 0;
    uint32_t _currentFrameInFlight = 0;

    VkSurfaceKHR _surface = VK_NULL_HANDLE;

    VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
    VkSwapchainCreateInfoKHR _swapchainCreateInfo = {};
    std::vector<VkImageView> _swapchainImageViews;

    VkImage _depthStencil = VK_NULL_HANDLE;
    VkImageView _depthStencilView = VK_NULL_HANDLE;
    VkDeviceMemory _depthStencilMemory = VK_NULL_HANDLE;

    bool _isInitialized = false;

    // extensions
    bool _hasDebugUtils = false;
    bool _hasKHR_PORTABILITY_ENUMERATION = false;
    bool _hasKHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION = false;
    // bool _hasKHR_portability_subset = false;

    // validation layers
    bool _hasKHRONOS_validation = false;
    bool _hasKHRONOS_profiles = false;

    void initLoader();
    void deinitLoader();

    void initExtensions(std::vector<const char*> additionalExt = {});

    void initValidationLayers();

    void createInstance(const char* appName = "");

    void pickPhysicalDevice();

    void initMemProperties();

    void createDevice(std::vector<const char*> ext = {});

    void createQueue();

    void createCommandPool();

    void createCommandBuffers();

    void createSyncObjects();

    void createSurface(kame::sdl::WindowVk& window);

    void createSwapchain(VkExtent2D screenSize);

    void createSwapchain(kame::sdl::WindowVk& window);

    void createSwapchainImageViews();

    void createDefaultDepthStencil();

    void startup(kame::sdl::WindowVk& window);

    void destroyInstance();

    void destroyDevice();

    void destroyQueue();

    void destroyCommandPool();

    void destroyCommandBuffers();

    void destroySyncObjects();

    void destroySurface();

    void destroySwapchain();

    void destroySwapchainImageViews();

    void destroyDefaultDepthStencil();

    void shutdown();

    bool _findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, uint32_t& i, uint32_t& type);

    VkResult allocateMemory(const VkMemoryRequirements& memRequirements, VkMemoryPropertyFlags properties, VkDeviceMemory& deviceMemoryResult);

    void freeMemory(VkDeviceMemory& memory);

    void mapMemory(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, void** ppData);

    void unmapMemory(VkDeviceMemory memory);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& bufferResult, VkMemoryRequirements& memRequirementsResult);

    void destroyBuffer(VkBuffer& buffer);

    void bindBufferMemory(VkBuffer buffer, VkDeviceMemory deviceMemory, VkDeviceSize memoryOffset = 0);

    void createImage2D(VkExtent2D size, VkFormat format, VkImageUsageFlags usage, VkImage& imageResult, VkMemoryRequirements& memRequirementsResult);

    void destroyImage2D(VkImage& image);

    void bindImageMemory(VkImage image, VkDeviceMemory deviceMemory, VkDeviceSize memoryOffset = 0);

    void createImageView2D(VkImage image, VkFormat format, VkImageAspectFlags aspectMask, VkImageView& imageViewResult);

    void destroyImageView(VkImageView& imageView);

    void createShaderModule(const std::vector<char>&, VkShaderModule& shaderResult);

    void destroyShaderModule(VkShaderModule& shader);

    void createDescriptorPool(const std::vector<VkDescriptorPoolSize>& requestSize, VkDescriptorPoolCreateFlagBits flags, uint32_t maxSets, VkDescriptorPool& poolResult);

    void destroyDescriptorPool(VkDescriptorPool& pool);

    void createDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayoutCreateFlags flags, VkDescriptorSetLayout& layoutResult);

    void destroyDescriptorSetLayout(VkDescriptorSetLayout& layout);

    void createDescriptorSet(VkDescriptorPool& pool, VkDescriptorSetLayout& layout, VkDescriptorSet& setResult);

    void updateDescriptorSet(const std::vector<VkWriteDescriptorSet>& wdsList);

    void createGraphicPipeline(VkGraphicsPipelineCreateInfo& info, VkPipeline& pipelineResult);

    void destroyGraphicsPipeline(VkPipeline& pipeline);

    VkCommandBuffer _getCmdBuffer();

    void setMemoryBarrier(VkPipelineStageFlags src, VkPipelineStageFlags dst);

    void beginCmd();

    void endCmd();

    void cmdCopyBuffer(VkBuffer src, VkBuffer dst, const VkBufferCopy& region);

    VkQueue _getQueue();

    VkFence _getFence();

    void submitCommands(bool wait = true);
};

} // namespace kame::vk

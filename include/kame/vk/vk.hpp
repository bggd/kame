#pragma once

#include "volk_header.hpp"

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

    VkRenderPass _renderPass = VK_NULL_HANDLE;

    std::vector<VkFramebuffer> _framebuffers;

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

    void initInstance(const char* appName = "");

    void pickPhysicalDevice();

    void initMemProperties();

    void initDevice(std::vector<const char*> ext = {});

    void initQueue();

    void initCommandPool();

    void initCommandBuffers();

    void initSyncObjects();

    void initSurface(kame::sdl::WindowVk& window);

    void initSwapchain(VkExtent2D screenSize);

    void initSwapchain(kame::sdl::WindowVk& window);

    void initSwapchainImageViews();

    void initDefaultDepthStencil();

    void initDefaultRenderPass();

    void initDefaultFramebuffers();

    void startup(kame::sdl::WindowVk& window);

    void deinitInstance();

    void deinitDevice();

    void deinitQueue();

    void deinitCommandPool();

    void deinitCommandBuffers();

    void deinitSyncObjects();

    void deinitSurface();

    void deinitSwapchain();

    void deinitSwapchainImageViews();

    void deinitDefaultDepthStencil();

    void deinitDefaultRenderPass();

    void deinitDefaultFramebuffers();

    void shutdown();

    bool _findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, uint32_t& i, uint32_t& type);

    [[nodiscard]] VkDeviceMemory allocateDeviceMemory(const VkMemoryRequirements& memRequirements, VkMemoryPropertyFlags properties, VkResult& result);
    [[nodiscard]] VkDeviceMemory allocateDeviceMemory(const VkMemoryRequirements& memRequirements, VkMemoryPropertyFlags properties)
    {
        VkResult r{};
        return allocateDeviceMemory(memRequirements, properties, r);
    };

    void freeDeviceMemory(VkDeviceMemory& memory);

    void memcpyDeviceMemory(VkDeviceMemory memory, const void* srcData, VkDeviceSize size);

    [[nodiscard]] VkBuffer createBuffer(const VkBufferCreateInfo& info);

    [[nodiscard]] VkMemoryRequirements getBufferMemoryRequirements(VkBuffer& buffer);

    void destroyBuffer(VkBuffer& buffer);

    void bindBufferMemory(VkBuffer buffer, VkDeviceMemory deviceMemory, VkDeviceSize memoryOffset = 0);

    [[nodiscard]] VkImage createImage(const VkImageCreateInfo& info);

    [[nodiscard]] VkMemoryRequirements getImageMemoryRequirements(VkImage image);

    [[nodiscard]] VkImage createImage2D(VkExtent2D size, VkFormat format, VkImageUsageFlags usage);

    void destroyImage(VkImage& image);

    void bindImageMemory(VkImage image, VkDeviceMemory deviceMemory, VkDeviceSize memoryOffset = 0);

    [[nodiscard]] VkImageView createImageView(const VkImageViewCreateInfo& info);

    [[nodiscard]] VkImageView createImageView2D(VkImage image, VkFormat format, VkImageAspectFlags aspectMask);

    void destroyImageView(VkImageView& imageView);

    [[nodiscard]] VkShaderModule createShaderModule(const std::vector<char>& code);

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

    void _beginCmd();

    void _endCmd();

    void cmdPipelineBarrier(VkPipelineStageFlags src, VkPipelineStageFlags dst);

    void cmdMemoryBarrier(VkPipelineStageFlags src, VkPipelineStageFlags dst, VkAccessFlags srcAccess, VkAccessFlags dstAccess);

    void cmdCopyBuffer(VkBuffer src, VkBuffer dst, const VkBufferCopy& region);

    VkQueue _getQueue();

    VkFence _getFence();

    void submitCmds(bool waitFence = true);
};

} // namespace kame::vk

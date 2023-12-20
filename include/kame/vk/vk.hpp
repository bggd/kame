#pragma once

#include "vk_header.hpp"

#include <utility>
#include <vector>
#include <string>

#include <kame/sdl/sdl.hpp>

namespace kame::vk {

struct Command {
    VkCommandPool pool = VK_NULL_HANDLE;
    VkCommandBuffer buffer = VK_NULL_HANDLE;
};

struct Device {
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;
};

struct Queue {
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamilyIndex = 0;
    uint32_t presentQueueFamilyIndex = 0;
};

struct SwapChain {
    VkSwapchainKHR swapChainKHR = VK_NULL_HANDLE;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
};

struct Vulkan {
    VkInstance instance = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    Device* device = nullptr;
    Queue* queue = nullptr;
    SwapChain* swapChain = nullptr;
    Command* mainCommand = nullptr;
    std::vector<std::string> avaliableExtensions;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    std::vector<std::string> avaliableLayers;
    std::vector<const char*> validationLayers;
    bool isDebugMode = false;

    void setDebugMode(bool debug) { isDebugMode = debug; }
    std::vector<std::string> getInstanceExtensions();
    bool hasInstanceExtension(std::string name);
    void createInstance(const char* appName = "");
    void createSurface(const kame::sdl::WindowVk* window);
    void createSurface(const kame::sdl::WindowVk& window) { createSurface(&window); }
    void createDeviceAndQueue();
    void createSwapChain(const kame::sdl::WindowVk* window);
    void createSwapChain(const kame::sdl::WindowVk& window) { return createSwapChain(&window); }
    void createMainCommand();

    void destroy();
};

} // namespace kame::vk

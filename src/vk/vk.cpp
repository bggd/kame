#include <all.hpp>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

namespace kame::vk {

std::vector<std::string> getValidationLayers()
{
    uint32_t count = 0;
    VK_CHECK_INCOMPLETE(vkEnumerateInstanceLayerProperties(&count, nullptr));
    if (count == 0)
    {
        return {};
    }
    std::vector<VkLayerProperties> properties(count);
    VK_CHECK_INCOMPLETE(vkEnumerateInstanceLayerProperties(&count, properties.data()));
    std::vector<std::string> layers(count);
    for (size_t i = 0; i < count; ++i)
    {
        layers[i] = std::string(properties[i].layerName);
    }
    return layers;
}

std::vector<std::string> getDeviceExtensions(VkPhysicalDevice device)
{
    uint32_t count = 0;
    VK_CHECK_INCOMPLETE(vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr));
    if (count == 0)
    {
        return {};
    }
    std::vector<VkExtensionProperties> properties(count);
    VK_CHECK_INCOMPLETE(vkEnumerateDeviceExtensionProperties(device, nullptr, &count, properties.data()));
    std::vector<std::string> extensions(count);
    for (size_t i = 0; i < count; ++i)
    {
        extensions[i] = std::string(properties[i].extensionName);
    }
    return extensions;
}

std::vector<std::string> Vulkan::getInstanceExtensions()
{
    uint32_t count = 0;
    VK_CHECK_INCOMPLETE(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
    if (count == 0)
    {
        return {};
    }
    std::vector<VkExtensionProperties> properties(count);
    VK_CHECK_INCOMPLETE(vkEnumerateInstanceExtensionProperties(nullptr, &count, properties.data()));
    std::vector<std::string> extensions(count);
    for (size_t i = 0; i < count; ++i)
    {
        extensions[i] = std::string(properties[i].extensionName);
    }
    return extensions;
}

bool Vulkan::hasInstanceExtension(std::string name)
{
    for (const auto& i : avaliableExtensions)
    {
        if (i == name)
        {
            return true;
        }
    }
    return false;
}

void Vulkan::createInstance(const char* appName)
{
    assert(!instance);

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = 0;
    appInfo.pEngineName = nullptr;
    appInfo.engineVersion = 0;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&createInfo.enabledExtensionCount);
    std::vector<const char*> extensions(sdlExtensions, sdlExtensions + createInfo.enabledExtensionCount);
    avaliableExtensions = getInstanceExtensions();
    for (size_t i = 0; i < avaliableExtensions.size(); ++i)
    {
        SPDLOG_INFO("Instance Extension: {}", avaliableExtensions[i]);
    }

    VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
    if (isDebugMode && hasInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
    {
        debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
        debugInfo.pfnUserCallback = debugCallback;

        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        avaliableLayers = getValidationLayers();
        for (const auto& i : avaliableLayers)
        {
            SPDLOG_INFO("Validation Layer: {}", i);
        }
        validationLayers = {"VK_LAYER_KHRONOS_validation", "VK_LAYER_KHRONOS_profiles"};
        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugInfo;
    }

    if (hasInstanceExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
    {
#if defined(VK_USE_PLATFORM_MACOS_MVK) && (VK_HEADER_VERSION >= 216)
        extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
    }
    if (hasInstanceExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
    {
        extensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    }
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance));
    volkLoadInstance(instance);

    if (isDebugMode && hasInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
    {
        assert(!debugMessenger);
        VK_CHECK(vkCreateDebugUtilsMessengerEXT(instance, &debugInfo, nullptr, &debugMessenger));
    }
}

void Vulkan::createSurface(const kame::sdl::WindowVk* window)
{
    assert(!surface);
    assert(window->window);

    SDL_Vulkan_CreateSurface(window->window, instance, nullptr, &surface);
    assert(surface);
}

void Vulkan::createDeviceAndQueue()
{
    assert(!device);
    assert(!queue);

    device = new Device();
    queue = new Queue();

    uint32_t deviceCount = 0;
    VK_CHECK_INCOMPLETE(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));
    assert(deviceCount);

    std::vector<VkPhysicalDevice> devices(deviceCount);
    VK_CHECK_INCOMPLETE(vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()));

    int prevScore = -1;
    for (const auto& d : devices)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(d, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(d, &deviceFeatures);

        int score = 0;
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 100;
        }
        else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            score += 50;
        }

        score += deviceProperties.limits.maxImageDimension2D;

        if (deviceFeatures.geometryShader)
        {
            score += 10;
        }

        bool hasSwapchainExt = false;
        std::vector<std::string> availableExtensions = getDeviceExtensions(d);
        for (const auto& ext : availableExtensions)
        {
            if (ext == std::string(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
            {
                hasSwapchainExt = true;
            }
        }
        if (!hasSwapchainExt)
        {
            continue;
        }

        if (score > prevScore)
        {
            device->physicalDevice = d;
        }
        prevScore = score;
    }
    assert(device->physicalDevice);

    // Select Queue
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device->physicalDevice, &queueFamilyCount, nullptr);
    assert(queueFamilyCount);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device->physicalDevice, &queueFamilyCount, queueFamilies.data());

    uint32_t graphicsQueueIndex = 0;
    uint32_t presentQueueIndex = 0;
    uint32_t index = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsQueueIndex = index;
        }
        VkBool32 supported;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device->physicalDevice, index, surface, &supported));
        if (supported)
        {
            presentQueueIndex = index;
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                break;
            }
        }
        ++index;
    }
    assert(queueFamilies[graphicsQueueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT);
    VkBool32 supported;
    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device->physicalDevice, presentQueueIndex, surface, &supported));
    assert(supported);

    float queuePriority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfoList;
    if (graphicsQueueIndex == presentQueueIndex)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = graphicsQueueIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfoList.emplace_back(queueCreateInfo);
    }
    else
    {
        VkDeviceQueueCreateInfo q1{};
        q1.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        q1.queueFamilyIndex = graphicsQueueIndex;
        q1.queueCount = 1;
        q1.pQueuePriorities = &queuePriority;

        queueCreateInfoList.emplace_back(q1);

        VkDeviceQueueCreateInfo q2{};
        q2.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        q2.queueFamilyIndex = presentQueueIndex;
        q2.queueCount = 1;
        q2.pQueuePriorities = &queuePriority;

        queueCreateInfoList.emplace_back(q2);
    }

    // Create logical device
    std::vector<const char*> deviceExtensionNames = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    auto avaliableDeviceExtension = getDeviceExtensions(device->physicalDevice);
    for (const auto& ext : avaliableDeviceExtension)
    {
        SPDLOG_INFO("Device Extension: {}", ext);
        if (ext == std::string("VK_KHR_portability_subset") && hasInstanceExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
        {
            deviceExtensionNames.emplace_back("VK_KHR_portability_subset");
        }
    }
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device->physicalDevice, &deviceFeatures);
    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pQueueCreateInfos = queueCreateInfoList.data();
    deviceInfo.queueCreateInfoCount = queueCreateInfoList.size();
    deviceInfo.pEnabledFeatures = &deviceFeatures;
    deviceInfo.enabledExtensionCount = deviceExtensionNames.size();
    deviceInfo.ppEnabledExtensionNames = deviceExtensionNames.data();
    if (isDebugMode)
    {
        deviceInfo.enabledLayerCount = validationLayers.size();
        deviceInfo.ppEnabledLayerNames = validationLayers.data();
    }

    VK_CHECK(vkCreateDevice(device->physicalDevice, &deviceInfo, nullptr, &device->logicalDevice));
    volkLoadDevice(device->logicalDevice);

    vkGetDeviceQueue(device->logicalDevice, graphicsQueueIndex, 0, &queue->graphicsQueue);
    assert(queue->graphicsQueue);
    if (graphicsQueueIndex == presentQueueIndex)
    {
        queue->presentQueue = queue->graphicsQueue;
    }
    else
    {
        vkGetDeviceQueue(device->logicalDevice, presentQueueIndex, 0, &queue->presentQueue);
        assert(queue->presentQueue);
    }
    queue->graphicsQueueFamilyIndex = graphicsQueueIndex;
    queue->presentQueueFamilyIndex = presentQueueIndex;
}

void Vulkan::createSwapChain(const kame::sdl::WindowVk* window)
{
    assert(window->window);
    assert(!swapChain);

    swapChain = new SwapChain();

    VkSurfaceCapabilitiesKHR capabilities;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->physicalDevice, surface, &capabilities));
    uint32_t formatCount;
    VK_CHECK_INCOMPLETE(vkGetPhysicalDeviceSurfaceFormatsKHR(device->physicalDevice, surface, &formatCount, nullptr));
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    if (formatCount != 0)
    {
        VK_CHECK_INCOMPLETE(vkGetPhysicalDeviceSurfaceFormatsKHR(device->physicalDevice, surface, &formatCount, formats.data()));
    }
    uint32_t presentModeCount;
    VK_CHECK_INCOMPLETE(vkGetPhysicalDeviceSurfacePresentModesKHR(device->physicalDevice, surface, &presentModeCount, nullptr));
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    if (presentModeCount != 0)
    {
        VK_CHECK_INCOMPLETE(vkGetPhysicalDeviceSurfacePresentModesKHR(device->physicalDevice, surface, &presentModeCount, presentModes.data()));
    }
    assert(!formats.empty());
    assert(!presentModes.empty());

    VkSurfaceFormatKHR surfaceFormat;
    if (formatCount == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
    {
        surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        surfaceFormat.format = VK_FORMAT_R8G8B8A8_UNORM;
    }
    else
    {
        surfaceFormat = formats[0];
    }

    int x, y;
    SDL_GetWindowSizeInPixels(window->window, &x, &y);
    assert(x > 0 && y > 0);

    VkExtent2D swapChainSize;
    swapChainSize.width = SDL_clamp((uint32_t)x,
                                    capabilities.minImageExtent.width,
                                    capabilities.maxImageExtent.width);
    swapChainSize.height = SDL_clamp((uint32_t)y,
                                     capabilities.minImageExtent.height,
                                     capabilities.maxImageExtent.height);

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapChainInfo{};
    swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainInfo.surface = surface;
    swapChainInfo.minImageCount = imageCount;
    swapChainInfo.imageFormat = surfaceFormat.format;
    swapChainInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainInfo.imageExtent = swapChainSize;
    swapChainInfo.imageArrayLayers = 1;
    swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (queue->graphicsQueue == queue->presentQueue)
    {
        swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    else
    {
        // swapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        // swapChainInfo.queueFamilyIndexCount = 2;
        // swapChainInfo.pQueueFamilyIndices = ;
    }

    swapChainInfo.preTransform = capabilities.currentTransform;

    swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    swapChainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapChainInfo.clipped = VK_TRUE;

    swapChainInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(device->logicalDevice, &swapChainInfo, nullptr, &swapChain->swapChainKHR));

    VK_CHECK_INCOMPLETE(vkGetSwapchainImagesKHR(device->logicalDevice, swapChain->swapChainKHR, &imageCount, nullptr));
    swapChain->images.resize(imageCount);
    VK_CHECK_INCOMPLETE(vkGetSwapchainImagesKHR(device->logicalDevice, swapChain->swapChainKHR, &imageCount, swapChain->images.data()));

    swapChain->imageViews.resize(imageCount);
    for (size_t i = 0; i < swapChain->images.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChain->images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = surfaceFormat.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        VK_CHECK(vkCreateImageView(device->logicalDevice, &createInfo, nullptr, &swapChain->imageViews[i]));
    }
}

void Vulkan::createMainCommand()
{
    assert(!mainCommand);

    mainCommand = new Command();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queue->graphicsQueueFamilyIndex;

    VK_CHECK(vkCreateCommandPool(device->logicalDevice, &poolInfo, nullptr, &mainCommand->pool));

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = mainCommand->pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VK_CHECK(vkAllocateCommandBuffers(device->logicalDevice, &allocInfo, &mainCommand->buffer));
}

void Vulkan::destroy()
{
    assert(instance);
    assert(swapChain);
    assert(device);
    assert(device->logicalDevice);
    assert(mainCommand);
    assert(mainCommand->pool);

    vkDestroyCommandPool(device->logicalDevice, mainCommand->pool, nullptr);

    for (auto imageView : swapChain->imageViews)
    {
        vkDestroyImageView(device->logicalDevice, imageView, nullptr);
    }
    assert(swapChain->swapChainKHR);
    vkDestroySwapchainKHR(device->logicalDevice, swapChain->swapChainKHR, nullptr);
    vkDestroyDevice(device->logicalDevice, nullptr);
    if (isDebugMode && hasInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
    {
        assert(debugMessenger);
        vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    assert(surface);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

} // namespace kame::vk

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    [[maybe_unused]] void* pUserData)
{

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        SPDLOG_ERROR("Validation layer: {}", pCallbackData->pMessage);
    }
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        SPDLOG_WARN("Validation layer: {}", pCallbackData->pMessage);
    }

    return VK_FALSE;
}

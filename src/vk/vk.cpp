#include <all.hpp>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

static const char* VK_LAYER_KHRONOS_validation_NAME = "VK_LAYER_KHRONOS_validation";
static const char* VK_LAYER_KHRONOS_profiles_NAME = "VK_LAYER_KHRONOS_profiles";

static const char* VK_KHR_portability_subset_NAME = "VK_KHR_portability_subset";

namespace kame::vk {

void Vulkan::initLoader()
{
    VK_CHECK(volkInitialize());

    uint32_t version = volkGetInstanceVersion();
    SPDLOG_INFO("Vulkan Version: {}.{}.{}",
                VK_VERSION_MAJOR(version),
                VK_VERSION_MINOR(version),
                VK_VERSION_PATCH(version));
}

void Vulkan::deinitLoader()
{
    volkFinalize();
}

void Vulkan::initExtensions(std::vector<const char*> additionalExt)
{
    uint32_t count = 0;

    VK_CHECK_INCOMPLETE(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));

    if (count == 0)
    {
        return;
    }

    _extensionProperties.resize(count);

    VK_CHECK_INCOMPLETE(vkEnumerateInstanceExtensionProperties(nullptr, &count, _extensionProperties.data()));

    _hasDebugUtils = false;
    _hasKHR_PORTABILITY_ENUMERATION = false;
    _hasKHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION = false;

    for (const auto& p : _extensionProperties)
    {
        if (!_hasDebugUtils && SDL_strcmp(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, p.extensionName))
        {
            _hasDebugUtils = true;
            _extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        if (!_hasKHR_PORTABILITY_ENUMERATION && SDL_strcmp(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, p.extensionName))
        {
            _hasKHR_PORTABILITY_ENUMERATION = true;
#if defined(VK_USE_PLATFORM_MACOS_MVK) && (VK_HEADER_VERSION >= 216)
            _extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
        }
        if (!_hasKHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION && SDL_strcmp(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, p.extensionName))
        {
            _hasKHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION = true;
            _extensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        }
    }

    for (auto p : additionalExt)
    {
        _extensions.emplace_back(p);
    }
}

void Vulkan::initValidationLayers()
{
    uint32_t count = 0;

    VK_CHECK_INCOMPLETE(vkEnumerateInstanceLayerProperties(&count, nullptr));

    if (count == 0)
    {
        return;
    }

    _layerProperties.resize(count);

    VK_CHECK_INCOMPLETE(vkEnumerateInstanceLayerProperties(&count, _layerProperties.data()));

    std::vector<std::string> layers(count);

    for (const auto& p : _layerProperties)
    {
        if (!_hasKHRONOS_validation && SDL_strcmp(VK_LAYER_KHRONOS_validation_NAME, p.layerName))
        {
            _hasKHRONOS_validation = true;
        }
        if (!_hasKHRONOS_profiles && SDL_strcmp(VK_LAYER_KHRONOS_profiles_NAME, p.layerName))
        {
            _hasKHRONOS_profiles = true;
        }
    }

    if (_hasKHRONOS_validation)
    {
        _validationLayers.emplace_back(VK_LAYER_KHRONOS_validation_NAME);
    }
    if (_hasKHRONOS_profiles)
    {
        _validationLayers.emplace_back(VK_LAYER_KHRONOS_profiles_NAME);
    }

    _debugInfo = VkDebugUtilsMessengerCreateInfoEXT{};

    _debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    _debugInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    _debugInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;

    _debugInfo.pfnUserCallback = debugCallback;
}

void Vulkan::initInstance(const char* appName)
{
    VkApplicationInfo ai{};
    ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    ai.apiVersion = VK_API_VERSION_1_0;

    ai.pApplicationName = appName;

    VkInstanceCreateInfo ici{};
    ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    ici.pApplicationInfo = &ai;

    ici.ppEnabledExtensionNames = _extensions.data();
    ici.enabledExtensionCount = _extensions.size();

    ici.ppEnabledLayerNames = _validationLayers.data();
    ici.enabledLayerCount = _validationLayers.size();

    if (_hasDebugUtils)
    {
        ici.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&_debugInfo;
    }

#if defined(VK_USE_PLATFORM_MACOS_MVK) && (VK_HEADER_VERSION >= 216)
    ici.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    assert(!_instance);

    VK_CHECK(vkCreateInstance(&ici, nullptr, &_instance));

    volkLoadInstance(_instance);

    if (_hasDebugUtils)
    {
        assert(!_debugMessanger);

        VK_CHECK(vkCreateDebugUtilsMessengerEXT(_instance, &_debugInfo, nullptr, &_debugMessanger));
    }
}

void Vulkan::initMemProperties()
{
    _memProperties = VkPhysicalDeviceMemoryProperties{};

    vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &_memProperties);
}

void Vulkan::pickPhysicalDevice()
{
    assert(!_physicalDevice);

    uint32_t deviceCount;

    VK_CHECK_INCOMPLETE(vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr));

    std::vector<VkPhysicalDevice> devices(deviceCount);

    VK_CHECK_INCOMPLETE(vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data()));

    assert(deviceCount);

    VkPhysicalDevice pick = VK_NULL_HANDLE;

    uint32_t queueCount = 0;

    std::vector<VkQueueFamilyProperties> properties;

    for (uint32_t i = 0; i < deviceCount; ++i)
    {
        VkPhysicalDeviceProperties dp;

        vkGetPhysicalDeviceProperties(devices[i], &dp);

        if (dp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueCount, nullptr);
            properties.resize(queueCount);
            vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueCount, properties.data());

            for (uint32_t i = 0; i < queueCount; ++i)
            {
                const auto& family = properties[i];

                VkBool32 presentSupport;
                VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(devices[i], i, _surface, &presentSupport));

                if (!presentSupport)
                {
                    continue;
                }

                if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    pick = devices[i];

                    _qFamilyGraphicsIndex = i;

                    break;
                }
            }
        }
    }

    if (!pick)
    {
        for (uint32_t i = 0; i < deviceCount; ++i)
        {
            VkPhysicalDeviceProperties dp;

            vkGetPhysicalDeviceProperties(devices[i], &dp);

            vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueCount, nullptr);
            properties.resize(queueCount);
            vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueCount, properties.data());

            for (uint32_t i = 0; i < queueCount; ++i)
            {
                const auto& family = properties[i];

                VkBool32 presentSupport;
                VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(devices[i], i, _surface, &presentSupport));

                if (!presentSupport)
                {
                    continue;
                }

                if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    pick = devices[i];

                    _qFamilyGraphicsIndex = i;

                    break;
                }
            }
        }
    }

    assert(pick);

    _physicalDevice = pick;
}

void Vulkan::initDevice(std::vector<const char*> ext)
{
    float priority = 1.0f;

    VkDeviceQueueCreateInfo qci_0{};
    qci_0.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    qci_0.pQueuePriorities = &priority;
    qci_0.queueCount = 1;
    qci_0.queueFamilyIndex = _qFamilyGraphicsIndex;

    std::vector<VkDeviceQueueCreateInfo> qciInfos = {qci_0};

    VkDeviceCreateInfo dci{};
    dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    dci.queueCreateInfoCount = qciInfos.size();
    dci.pQueueCreateInfos = qciInfos.data();

    dci.enabledLayerCount = _validationLayers.size();
    dci.ppEnabledLayerNames = _validationLayers.data();

    if (_hasKHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION)
    {

        uint32_t count;
        VK_CHECK_INCOMPLETE(vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &count, nullptr));
        std::vector<VkExtensionProperties> properties(count);
        VK_CHECK_INCOMPLETE(vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &count, properties.data()));

        for (const auto& p : properties)
        {
            if (SDL_strcmp(VK_KHR_portability_subset_NAME, p.extensionName))
            {
                ext.emplace_back(VK_KHR_portability_subset_NAME);
                break;
            }
        }
    }

    if (!ext.empty())
    {
        dci.enabledExtensionCount = ext.size();
        dci.ppEnabledExtensionNames = ext.data();
    }

    VK_CHECK(vkCreateDevice(_physicalDevice, &dci, nullptr, &_device));

    volkLoadDevice(_device);
}

void Vulkan::initQueue()
{
    assert(!_graphicsQueue);

    vkGetDeviceQueue(_device, _qFamilyGraphicsIndex, 0, &_graphicsQueue);
}

void Vulkan::initCommandPool()
{
    assert(!_commandPool);

    VkCommandPoolCreateInfo cpci{};
    cpci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    cpci.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    cpci.queueFamilyIndex = _qFamilyGraphicsIndex;

    VK_CHECK(vkCreateCommandPool(_device, &cpci, nullptr, &_commandPool));
}

void Vulkan::initCommandBuffers()
{
    assert(_cmdBuffers.empty());

    _cmdBuffers.resize(_numFramesInFlight);

    VkCommandBufferAllocateInfo cbai{};
    cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    cbai.commandPool = _commandPool;

    cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    cbai.commandBufferCount = _cmdBuffers.size();

    VK_CHECK(vkAllocateCommandBuffers(_device, &cbai, _cmdBuffers.data()));
}

void Vulkan::initSyncObjects()
{
    assert(_inFlightFences.empty());

    _inFlightFences.resize(_numFramesInFlight);

    VkFenceCreateInfo fci{};
    fci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    fci.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < _inFlightFences.size(); ++i)
    {
        VK_CHECK(vkCreateFence(_device, &fci, nullptr, &_inFlightFences[i]));
    }
}

void Vulkan::initSurface(kame::sdl::WindowVk& window)
{
    assert(!_surface);

    assert(SDL_Vulkan_CreateSurface(window.window, _instance, nullptr, &_surface));
}

void Vulkan::initSwapchain(VkExtent2D screenSize)
{
    VkSurfaceCapabilitiesKHR caps{};

    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface, &caps));

    uint32_t formatCount = 0;

    VK_CHECK_INCOMPLETE(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, nullptr));

    std::vector<VkSurfaceFormatKHR> formats(formatCount);

    VK_CHECK_INCOMPLETE(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, formats.data()));

    assert(!formats.empty());

    uint32_t modeCount = 0;

    VK_CHECK_INCOMPLETE(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &modeCount, nullptr));

    std::vector<VkPresentModeKHR> presentModes(modeCount);

    VK_CHECK_INCOMPLETE(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &modeCount, presentModes.data()));

    assert(!presentModes.empty());

    VkSurfaceFormatKHR surfaceFormat = {};

    if (formatCount == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
    {
        surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
        surfaceFormat.colorSpace = formats[0].colorSpace;
    }
    else
    {
        surfaceFormat = formats[0];
    }

    VkExtent2D swapChainSize;
    swapChainSize.width = SDL_clamp(screenSize.width,
                                    caps.minImageExtent.width,
                                    caps.maxImageExtent.width);
    swapChainSize.height = SDL_clamp(screenSize.height,
                                     caps.minImageExtent.height,
                                     caps.maxImageExtent.height);

    uint32_t imageCount = caps.minImageCount + 1;
    if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
    {
        imageCount = caps.maxImageCount;
    }

    VkSwapchainCreateInfoKHR sci{};
    sci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    sci.surface = _surface;

    sci.minImageCount = imageCount;
    sci.imageFormat = surfaceFormat.format;
    sci.imageColorSpace = surfaceFormat.colorSpace;
    sci.imageExtent = swapChainSize;
    sci.imageArrayLayers = 1;
    sci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    sci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    sci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    sci.preTransform = caps.currentTransform;
    sci.clipped = VK_TRUE;
    sci.oldSwapchain = VK_NULL_HANDLE;

    for (auto mode : presentModes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            sci.presentMode = mode;
            break;
        }
        else if (mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
        {
            sci.presentMode = mode;
            break;
        }
        else if (mode == VK_PRESENT_MODE_FIFO_KHR)
        {
            sci.presentMode = mode;
            break;
        }
    }

    VK_CHECK(vkCreateSwapchainKHR(_device, &sci, nullptr, &_swapchain));

    _swapchainCreateInfo = sci;
}

void Vulkan::initSwapchain(kame::sdl::WindowVk& window)
{
    int x, y;
    SDL_GetWindowSizeInPixels(window.window, &x, &y);
    assert(x > 0 && y > 0);

    VkExtent2D screenSize;
    screenSize.width = (uint32_t)x;
    screenSize.height = (uint32_t)y;

    initSwapchain(screenSize);
}

void Vulkan::initSwapchainImageViews()
{
    uint32_t count = 0;
    VK_CHECK_INCOMPLETE(vkGetSwapchainImagesKHR(_device, _swapchain, &count, nullptr));

    assert(count >= _swapchainCreateInfo.minImageCount);

    std::vector<VkImage> images(count);
    VK_CHECK_INCOMPLETE(vkGetSwapchainImagesKHR(_device, _swapchain, &count, images.data()));

    _swapchainImageViews.resize(count);

    for (size_t i = 0; i < _swapchainImageViews.size(); ++i)
    {
        VkImageViewCreateInfo ivci{};
        ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ivci.image = images[i];
        ivci.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ivci.format = _swapchainCreateInfo.imageFormat;
        ivci.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ivci.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ivci.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ivci.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ivci.subresourceRange.baseMipLevel = 0;
        ivci.subresourceRange.levelCount = 1;
        ivci.subresourceRange.baseArrayLayer = 0;
        ivci.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(_device, &ivci, nullptr, &_swapchainImageViews[i]));
    }
}

void Vulkan::initDefaultDepthStencil()
{
    VkExtent2D size{};

    size.width = _swapchainCreateInfo.imageExtent.width;
    size.height = _swapchainCreateInfo.imageExtent.height;

    VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    _depthStencil = createImage2D(size, VK_FORMAT_D32_SFLOAT, usage);

    VkMemoryRequirements memReq = getImageMemoryRequirements(_depthStencil);

    _depthStencilMemory = allocateDeviceMemory(memReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    bindImageMemory(_depthStencil, _depthStencilMemory);

    _depthStencilView = createImageView2D(_depthStencil, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void Vulkan::initDefaultRenderPass()
{
    VkAttachmentDescription adColor{};
    adColor.format = _swapchainCreateInfo.imageFormat;
    adColor.samples = VK_SAMPLE_COUNT_1_BIT;
    adColor.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    adColor.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    adColor.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    adColor.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    adColor.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    adColor.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription adDS{};
    adDS.format = VK_FORMAT_D32_SFLOAT;
    adDS.samples = VK_SAMPLE_COUNT_1_BIT;
    adDS.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    adDS.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    adDS.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    adDS.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    adDS.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    adDS.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAr{};
    colorAr.attachment = 0;
    colorAr.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAr{};
    depthAr.attachment = 1;
    depthAr.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAr;
    subpass.pDepthStencilAttachment = &depthAr;

    std::vector<VkAttachmentDescription> attachments = {adColor, adDS};

    VkSubpassDependency colorDep = {};
    colorDep.srcSubpass = VK_SUBPASS_EXTERNAL;
    colorDep.dstSubpass = 0;
    colorDep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    colorDep.srcAccessMask = 0;
    colorDep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    colorDep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency depthDep = {};
    depthDep.srcSubpass = VK_SUBPASS_EXTERNAL;
    depthDep.dstSubpass = 0;
    depthDep.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depthDep.srcAccessMask = 0;
    depthDep.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depthDep.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::vector<VkSubpassDependency> dependencies = {colorDep, depthDep};

    VkRenderPassCreateInfo rpci{};
    rpci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    rpci.attachmentCount = attachments.size();
    rpci.pAttachments = attachments.data();
    rpci.subpassCount = 1;
    rpci.pSubpasses = &subpass;
    rpci.dependencyCount = dependencies.size();
    rpci.pDependencies = dependencies.data();

    VK_CHECK(vkCreateRenderPass(_device, &rpci, nullptr, &_renderPass));
}

void Vulkan::initDefaultFramebuffers()
{
    _framebuffers.resize(_swapchainImageViews.size());

    for (size_t i = 0; i < _swapchainImageViews.size(); ++i)
    {
        std::vector<VkImageView> attachments = {
            _swapchainImageViews[i],
            _depthStencilView};

        VkFramebufferCreateInfo fbci{};
        fbci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbci.renderPass = _renderPass;
        fbci.attachmentCount = attachments.size();
        fbci.pAttachments = attachments.data();
        fbci.width = _swapchainCreateInfo.imageExtent.width;
        fbci.height = _swapchainCreateInfo.imageExtent.height;
        fbci.layers = 1;

        VK_CHECK(vkCreateFramebuffer(_device, &fbci, nullptr, &_framebuffers[i]));
    }
}

void Vulkan::startup(kame::sdl::WindowVk& window)
{
    assert(!_isInitialized);

    _numFramesInFlight = KAME_VK_MAX_FRAMES_IN_FLIGHT;

    uint32_t count = 0;
    auto pExt = SDL_Vulkan_GetInstanceExtensions(&count);

    std::vector<const char*> ext(pExt, pExt + count);

    initLoader();

    initExtensions(ext);

    initValidationLayers();

    initInstance(SDL_GetWindowTitle(window.window));

    initSurface(window);

    pickPhysicalDevice();

    initMemProperties();

    initDevice({VK_KHR_SWAPCHAIN_EXTENSION_NAME});

    initQueue();

    initCommandPool();

    initCommandBuffers();

    initSyncObjects();

    initSwapchain(window);

    initSwapchainImageViews();

    initDefaultDepthStencil();

    initDefaultRenderPass();

    initDefaultFramebuffers();

    _isInitialized = true;

    _beginCmd();
}

void Vulkan::deinitInstance()
{
    if (_debugMessanger)
    {
        vkDestroyDebugUtilsMessengerEXT(_instance, _debugMessanger, nullptr);

        _debugMessanger = VK_NULL_HANDLE;
    }

    assert(_instance);

    vkDestroyInstance(_instance, nullptr);

    _instance = VK_NULL_HANDLE;
}

void Vulkan::deinitDevice()
{
    assert(_physicalDevice);

    _physicalDevice = VK_NULL_HANDLE;

    assert(_device);

    vkDestroyDevice(_device, nullptr);

    _device = VK_NULL_HANDLE;
}

void Vulkan::deinitQueue()
{
    assert(_graphicsQueue);

    _graphicsQueue = VK_NULL_HANDLE;
}

void Vulkan::deinitCommandPool()
{
    assert(_commandPool);

    vkDestroyCommandPool(_device, _commandPool, nullptr);
}

void Vulkan::deinitCommandBuffers()
{
    assert(!_cmdBuffers.empty());

    _cmdBuffers.clear();
}

void Vulkan::deinitSyncObjects()
{
    assert(!_inFlightFences.empty());

    for (auto& fence : _inFlightFences)
    {
        assert(fence);

        vkDestroyFence(_device, fence, nullptr);
    }

    _inFlightFences.clear();
}

void Vulkan::deinitSurface()
{
    assert(_surface);

    vkDestroySurfaceKHR(_instance, _surface, nullptr);

    _surface = VK_NULL_HANDLE;
}

void Vulkan::deinitSwapchain()
{
    assert(_swapchain);

    vkDestroySwapchainKHR(_device, _swapchain, nullptr);

    _swapchain = VK_NULL_HANDLE;
}

void Vulkan::deinitSwapchainImageViews()
{
    for (auto& view : _swapchainImageViews)
    {
        assert(view);

        destroyImageView(view);
    }

    _swapchainImageViews.clear();
}

void Vulkan::deinitDefaultDepthStencil()
{
    assert(_depthStencil);
    assert(_depthStencilView);
    assert(_depthStencilMemory);

    destroyImageView(_depthStencilView);

    _depthStencilView = VK_NULL_HANDLE;

    destroyImage(_depthStencil);

    _depthStencil = VK_NULL_HANDLE;

    freeDeviceMemory(_depthStencilMemory);

    _depthStencilMemory = VK_NULL_HANDLE;
}

void Vulkan::deinitDefaultRenderPass()
{
    assert(_renderPass);

    vkDestroyRenderPass(_device, _renderPass, nullptr);

    _renderPass = VK_NULL_HANDLE;
}

void Vulkan::deinitDefaultFramebuffers()
{
    for (auto& fb : _framebuffers)
    {
        assert(fb);

        vkDestroyFramebuffer(_device, fb, nullptr);
    }

    _framebuffers.clear();
}

void Vulkan::shutdown()
{
    assert(_isInitialized);

    deinitDefaultFramebuffers();

    deinitDefaultRenderPass();

    deinitDefaultDepthStencil();

    deinitSwapchainImageViews();

    deinitSwapchain();

    deinitSurface();

    deinitSyncObjects();

    deinitCommandBuffers();

    deinitCommandPool();

    deinitQueue();

    deinitDevice();

    deinitInstance();

    deinitLoader();

    _numFramesInFlight = 0;

    _isInitialized = false;
}

bool Vulkan::_findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, uint32_t& i, uint32_t& type)
{
    while (i < _memProperties.memoryTypeCount)
    {
        if (typeFilter & (1 << i))
        {
            if ((_memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                type = i;
                return true;
            }
        }

        ++i;
    }

    return false;
}

VkDeviceMemory Vulkan::allocateDeviceMemory(const VkMemoryRequirements& memRequirements, VkMemoryPropertyFlags properties, VkResult& result)
{
    VkDeviceMemory deviceMemory;

    VkMemoryAllocateInfo mai{};
    mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    mai.allocationSize = memRequirements.size;

    result = VK_ERROR_UNKNOWN;

    for (uint32_t i = 0; i < _memProperties.memoryTypeCount; ++i)
    {
        if (_findMemoryType(memRequirements.memoryTypeBits, properties, i, mai.memoryTypeIndex))
        {
            result = vkAllocateMemory(_device, &mai, nullptr, &deviceMemory);

            if (result == VK_SUCCESS)
            {
                break;
            }
        }
    }

    return deviceMemory;
}

void Vulkan::freeDeviceMemory(VkDeviceMemory& memory)
{
    assert(memory);

    vkFreeMemory(_device, memory, nullptr);

    memory = VK_NULL_HANDLE;
}

void Vulkan::memcpyDeviceMemory(VkDeviceMemory memory, const void* srcData, VkDeviceSize size)
{
    assert(memory);
    assert(srcData);

    void* mappedData = nullptr;

    VK_CHECK(vkMapMemory(_device, memory, 0, size, 0, &mappedData));

    std::memcpy(mappedData, srcData, size);

    vkUnmapMemory(_device, memory);
}

VkBuffer Vulkan::createBuffer(const VkBufferCreateInfo& info)
{
    VkBufferCreateInfo bci{};
    bci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bci.size = info.size;
    bci.usage = info.usage;
    bci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer buffer = VK_NULL_HANDLE;

    VK_CHECK(vkCreateBuffer(_device, &bci, nullptr, &buffer));

    return buffer;

    // vkGetBufferMemoryRequirements(_device, bufferResult, &memRequirementsResult);
}

VkMemoryRequirements Vulkan::getBufferMemoryRequirements(VkBuffer& buffer)
{
    assert(buffer);

    VkMemoryRequirements requirements{};

    vkGetBufferMemoryRequirements(_device, buffer, &requirements);

    return requirements;
}

void Vulkan::destroyBuffer(VkBuffer& buffer)
{
    assert(buffer);

    vkDestroyBuffer(_device, buffer, nullptr);

    buffer = VK_NULL_HANDLE;
}

void Vulkan::bindBufferMemory(VkBuffer buffer, VkDeviceMemory deviceMemory, VkDeviceSize memoryOffset)
{
    assert(buffer);
    assert(deviceMemory);

    VK_CHECK(vkBindBufferMemory(_device, buffer, deviceMemory, memoryOffset));
}

VkImage Vulkan::createImage(const VkImageCreateInfo& info)
{
    VkImageCreateInfo ici = info;
    ici.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

    VkImage image = VK_NULL_HANDLE;

    VK_CHECK(vkCreateImage(_device, &ici, nullptr, &image));

    return image;
}

VkMemoryRequirements Vulkan::getImageMemoryRequirements(VkImage image)
{
    assert(image);

    VkMemoryRequirements requirements{};

    vkGetImageMemoryRequirements(_device, image, &requirements);

    return requirements;
}

VkImage Vulkan::createImage2D(VkExtent2D size, VkFormat format, VkImageUsageFlags usage)
{
    VkImageCreateInfo ici{};
    ici.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

    ici.imageType = VK_IMAGE_TYPE_2D;
    ici.extent.width = size.width;
    ici.extent.height = size.height;
    ici.extent.depth = 1;
    ici.mipLevels = 1;
    ici.arrayLayers = 1;
    ici.format = format;
    ici.tiling = VK_IMAGE_TILING_OPTIMAL;
    ici.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ici.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ici.usage = usage;
    ici.samples = VK_SAMPLE_COUNT_1_BIT;

    return createImage(ici);
}

void Vulkan::destroyImage(VkImage& image)
{
    assert(image);

    vkDestroyImage(_device, image, nullptr);

    image = VK_NULL_HANDLE;
}

void Vulkan::bindImageMemory(VkImage image, VkDeviceMemory deviceMemory, VkDeviceSize memoryOffset)
{
    assert(image);
    assert(deviceMemory);

    VK_CHECK(vkBindImageMemory(_device, image, deviceMemory, memoryOffset));
}

VkImageView Vulkan::createImageView(const VkImageViewCreateInfo& info)
{
    VkImageViewCreateInfo ivci = info;
    ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    VkImageView view = VK_NULL_HANDLE;

    VK_CHECK(vkCreateImageView(_device, &ivci, nullptr, &view));

    return view;
}

VkImageView Vulkan::createImageView2D(VkImage image, VkFormat format, VkImageAspectFlags aspectMask)
{
    assert(image);

    VkImageViewCreateInfo ivci{};
    ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    ivci.image = image;
    ivci.format = format;

    ivci.viewType = VK_IMAGE_VIEW_TYPE_2D;

    ivci.subresourceRange.aspectMask = aspectMask;
    ivci.subresourceRange.baseMipLevel = 0;
    ivci.subresourceRange.levelCount = 1;
    ivci.subresourceRange.baseArrayLayer = 0;
    ivci.subresourceRange.layerCount = 1;

    return createImageView(ivci);
}

void Vulkan::destroyImageView(VkImageView& imageView)
{
    assert(imageView);

    vkDestroyImageView(_device, imageView, nullptr);

    imageView = VK_NULL_HANDLE;
}

VkShaderModule Vulkan::createShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo smci{};
    smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    smci.codeSize = code.size();
    smci.pCode = (const uint32_t*)code.data();

    VkShaderModule shaderModule = VK_NULL_HANDLE;

    VK_CHECK(vkCreateShaderModule(_device, &smci, nullptr, &shaderModule));

    return shaderModule;
}

void Vulkan::destroyShaderModule(VkShaderModule& shader)
{
    assert(shader);

    vkDestroyShaderModule(_device, shader, nullptr);

    shader = VK_NULL_HANDLE;
}

void Vulkan::createDescriptorPool(const std::vector<VkDescriptorPoolSize>& requestSize, VkDescriptorPoolCreateFlagBits flags, uint32_t maxSets, VkDescriptorPool& poolResult)
{
    assert(!poolResult);

    VkDescriptorPoolCreateInfo dpci{};
    dpci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    dpci.flags = flags;

    dpci.maxSets = maxSets;

    dpci.poolSizeCount = requestSize.size();
    dpci.pPoolSizes = requestSize.data();

    VK_CHECK(vkCreateDescriptorPool(_device, &dpci, nullptr, &poolResult));
}

void Vulkan::destroyDescriptorPool(VkDescriptorPool& pool)
{
    assert(pool);

    vkDestroyDescriptorPool(_device, pool, nullptr);

    pool = VK_NULL_HANDLE;
}

void Vulkan::createDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayoutCreateFlags flags, VkDescriptorSetLayout& layoutResult)
{
    assert(!layoutResult);

    VkDescriptorSetLayoutCreateInfo dslci{};
    dslci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    dslci.flags = flags;

    dslci.bindingCount = bindings.size();
    dslci.pBindings = bindings.data();

    VK_CHECK(vkCreateDescriptorSetLayout(_device, &dslci, nullptr, &layoutResult));
}

void Vulkan::destroyDescriptorSetLayout(VkDescriptorSetLayout& layout)
{
    assert(layout);

    vkDestroyDescriptorSetLayout(_device, layout, nullptr);

    layout = VK_NULL_HANDLE;
}

void Vulkan::createDescriptorSet(VkDescriptorPool& pool, VkDescriptorSetLayout& layout, VkDescriptorSet& setResult)
{
    assert(pool);
    assert(!setResult);

    VkDescriptorSetAllocateInfo dsai{};
    dsai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    dsai.descriptorPool = pool;

    dsai.descriptorSetCount = 1;
    dsai.pSetLayouts = &layout;

    VK_CHECK(vkAllocateDescriptorSets(_device, &dsai, &setResult));
}

void Vulkan::updateDescriptorSet(const std::vector<VkWriteDescriptorSet>& wdsList)
{
    vkUpdateDescriptorSets(_device, wdsList.size(), wdsList.data(), 0, nullptr);
}

void Vulkan::createGraphicPipeline(VkGraphicsPipelineCreateInfo& info, VkPipeline& pipelineResult)
{
    assert(!pipelineResult);

    VK_CHECK(vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &info, nullptr, &pipelineResult));
}

void Vulkan::destroyGraphicsPipeline(VkPipeline& pipeline)
{
    assert(pipeline);

    vkDestroyPipeline(_device, pipeline, nullptr);

    pipeline = VK_NULL_HANDLE;
}

VkCommandBuffer Vulkan::_getCmdBuffer()
{
    return _cmdBuffers[_currentFrameInFlight];
}

void Vulkan::_beginCmd()
{
    VkCommandBufferBeginInfo cbbi{};
    cbbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    cbbi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK(vkResetCommandBuffer(_getCmdBuffer(), 0));

    VK_CHECK(vkBeginCommandBuffer(_getCmdBuffer(), &cbbi));
}

void Vulkan::_endCmd()
{
    VK_CHECK(vkEndCommandBuffer(_getCmdBuffer()));
}

void Vulkan::cmdPipelineBarrier(VkPipelineStageFlags src, VkPipelineStageFlags dst)
{
    vkCmdPipelineBarrier(_getCmdBuffer(), src, dst, 0, 0, nullptr, 0, nullptr, 0, nullptr);
}

void Vulkan::cmdMemoryBarrier(VkPipelineStageFlags src, VkPipelineStageFlags dst, VkAccessFlags srcAccess, VkAccessFlags dstAccess)
{
    VkMemoryBarrier mb{};
    mb.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;

    mb.srcAccessMask = srcAccess;
    mb.dstAccessMask = dstAccess;

    vkCmdPipelineBarrier(_getCmdBuffer(), src, dst, 0, 1, &mb, 0, nullptr, 0, nullptr);
}

void Vulkan::cmdCopyBuffer(VkBuffer src, VkBuffer dst, const VkBufferCopy& region)
{
    assert(src);
    assert(dst);

    vkCmdCopyBuffer(_getCmdBuffer(), src, dst, 1, &region);
}

VkQueue Vulkan::_getQueue()
{
    return _graphicsQueue;
}

VkFence Vulkan::_getFence()
{
    return _inFlightFences[_currentFrameInFlight];
}

void Vulkan::submitCmds(bool waitFence)
{
    _endCmd();

    VkCommandBuffer cmd = _getCmdBuffer();

    VkFence fence = _getFence();

    VK_CHECK(vkResetFences(_device, 1, &fence));

    VkSubmitInfo si{};
    si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    si.commandBufferCount = 1;
    si.pCommandBuffers = &cmd;

    VK_CHECK(vkQueueSubmit(_getQueue(), 1, &si, fence));

    if (waitFence)
    {
        VK_CHECK(vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX));
    }

    _beginCmd();
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

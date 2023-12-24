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

void Vulkan::initExtensions()
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

    uint32_t extCount;

    auto sdlExt = SDL_Vulkan_GetInstanceExtensions(&extCount);

    if (sdlExt)
    {
        for (uint32_t i = 0; i < extCount; ++i)
        {
            _extensions.emplace_back(sdlExt[i]);
        }
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

void Vulkan::createInstance(kame::sdl::WindowVk& window)
{
    VkApplicationInfo ai{};
    ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ai.apiVersion = VK_API_VERSION_1_0;

    ai.pApplicationName = SDL_GetWindowTitle(window.window);

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
        vkGetPhysicalDeviceQueueFamilyProperties(devices[0], &queueCount, nullptr);
        properties.resize(queueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(devices[0], &queueCount, properties.data());

        for (uint32_t i = 0; i < queueCount; ++i)
        {
            const auto& family = properties[i];

            if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                pick = devices[0];

                _qFamilyGraphicsIndex = i;

                break;
            }
        }
    }

    assert(pick);

    _physicalDevice = pick;
}

void Vulkan::createDevice()
{
    float priority = 1.0f;

    VkDeviceQueueCreateInfo qci_0{};
    qci_0.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    qci_0.pQueuePriorities = &priority;
    qci_0.queueCount = 1;
    qci_0.queueFamilyIndex = _qFamilyGraphicsIndex;

    std::vector<VkDeviceQueueCreateInfo> qciInfos = {qci_0};

    VkPhysicalDeviceFeatures features{};

    VkDeviceCreateInfo dci{};
    dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    dci.queueCreateInfoCount = qciInfos.size();
    dci.pQueueCreateInfos = qciInfos.data();

    dci.enabledLayerCount = _validationLayers.size();
    dci.ppEnabledLayerNames = _validationLayers.data();

    std::vector<const char*> ext;

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

        dci.enabledExtensionCount = ext.size();
        dci.ppEnabledExtensionNames = ext.data();
    }

    dci.pEnabledFeatures = &features;

    VK_CHECK(vkCreateDevice(_physicalDevice, &dci, nullptr, &_device));
}

void Vulkan::createQueue()
{
    assert(!_graphicsQueue);

    vkGetDeviceQueue(_device, _qFamilyGraphicsIndex, 0, &_graphicsQueue);
}

void Vulkan::startup(kame::sdl::WindowVk& window)
{
    assert(!_isInitialized);

    initExtensions();

    initValidationLayers();

    createInstance(window);

    pickPhysicalDevice();

    createDevice();

    createQueue();

    _isInitialized = true;
}

void Vulkan::destroyInstance()
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

void Vulkan::destroyDevice()
{
    assert(_physicalDevice);

    _physicalDevice = VK_NULL_HANDLE;

    assert(_device);

    vkDestroyDevice(_device, nullptr);

    _device = VK_NULL_HANDLE;
}

void Vulkan::destroyQueue()
{
    assert(_graphicsQueue);

    _graphicsQueue = VK_NULL_HANDLE;
}

void Vulkan::shutdown()
{
    assert(_isInitialized);

    destroyQueue();

    destroyDevice();

    destroyInstance();

    _isInitialized = false;
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

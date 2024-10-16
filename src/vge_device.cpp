// headers
#include "vge_device.hpp"

// std
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

namespace vge {

/* Callback function for Vulkan debug messages.
 *
 * This function is called by Vulkan whenever a validation message needs to be
 * logged, It writes the validation message to std::cerr and returns VK_FALSE to
 * indicate that the call should not be aborted.
 */
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    [[maybe_unused]] void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

/* Create Vulkan debug messenger if present to create debug info
 *
 * Attempts to create a debug messenger using the
 * `vkCreateDebugUtilsMessengerEXT` function, which is retrieved using Vulkan's
 * `vkGetInstanceProcAddr`. If successful, it returns VK_SUCCESS, otherwise
 * VK_ERROR_EXTENSION_NOT_PRESENT if the function isn't available.
 */
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

/* Destroy Vulkan debug messenger
 *
 * This function destroys a previously created Vulkan debug messenger using the
 * `vkDestroyDebugUtilsMessengerEXT` function. It is retrieved similarly to
 * `CreateDebugUtilsMessengerEXT` using `vkGetInstanceProcAddr`.
 */
void DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

// NOTE: Class member functions

/* Initialize Vulkan device
 *
 * Constructor for VgeDevice class. It sets up the Vulkan instance, debug
 * messenger, window surface, and selects a physical device (GPU) and logical
 * device. It also creates a command pool for managing Vulkan command buffers.
 */
VgeDevice::VgeDevice(VgeWindow& window) // default constructor
    : m_properties{}
    , m_instance{}
    , m_debugMessenger()
    , m_window{ window }
    , m_commandPool{}
    , m_device_{}
    , m_surface_{}
    , m_graphicsQueue_{}
    , m_presentQueue_{}
{
    createInstance();      // create/initialize the Vulkan instance/library
    setupDebugMessenger(); // validation layers: debug=on, release=off
    createSurface();       // GLFW surface
    pickPhysicalDevice();  // choose physical GPU
    createLogicalDevice(); // Manages features of our GPU we want to use
    createCommandPool();   // TODO: add summary
}

/* Cleanup Vulkan resources
 *
 * Destructor for VgeDevice class. Cleans up all Vulkan resources including the
 * command pool, logical device, debug messenger (if validation layers are
 * enabled), window surface, and Vulkan instance.
 */
VgeDevice::~VgeDevice()
{
    vkDestroyCommandPool(m_device_, m_commandPool, nullptr);
    vkDestroyDevice(m_device_, nullptr);

    if (m_enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(m_instance, m_surface_, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

/* Create Vulkan instance
 *
 * This function sets up a Vulkan instance, which is the first step in
 * initializing Vulkan. It configures application information, required
 * extensions, and validation layers if enabled. Throws an exception if instance
 * creation fails.
 */
void VgeDevice::createInstance()
{
    if (m_enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Game Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Vulkan Game Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    std::vector<const char*> extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (m_enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
        createInfo.ppEnabledLayerNames = m_validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }

    hasGflwRequiredInstanceExtensions();
}

/* Selects a physical GPU for the Vulkan application
 *
 * This function chooses a suitable GPU by enumerating available physical
 * devices and checking if they meet the application's requirements.
 */
void VgeDevice::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::cout << "Device count: " << deviceCount << std::endl;
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    for (const VkPhysicalDevice& device : devices) {
        if (isDeviceSuitable(device)) {
            m_physicalDevice = device;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties);
    std::cout << "physical device: " << m_properties.deviceName << std::endl;
}

/* Creates a logical device to interface with the selected GPU
 *
 * This function creates a logical device for interacting with the physical GPU.
 * It enables the required features and retrieves the graphics and presentation
 * queues.
 */
void VgeDevice::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

    // might not really be necessary anymore because device specific validation
    // layers have been deprecated
    if (m_enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
        createInfo.ppEnabledLayerNames = m_validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_device_, indices.graphicsFamily, 0, &m_graphicsQueue_);
    vkGetDeviceQueue(m_device_, indices.presentFamily, 0, &m_presentQueue_);
}

/* Creates a command pool for managing command buffers
 *
 * This function creates a Vulkan command pool for allocating and managing
 * command buffers.
 */
void VgeDevice::createCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = findPhysicalQueueFamilies();

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags =
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(m_device_, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

/* Creates a Vulkan surface using GLFW
 *
 * This function creates a window surface using GLFW to interface with the
 * Vulkan instance.
 */
void VgeDevice::createSurface()
{
    m_window.createWindowSurface(m_instance, &m_surface_);
}

/* Checks if a physical device is suitable for the Vulkan application
 *
 * This function checks if the provided physical device supports the required
 * features, extensions, and queue families.
 */
bool VgeDevice::isDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate =
            !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return indices.isComplete() && extensionsSupported && swapChainAdequate &&
           supportedFeatures.samplerAnisotropy;
}

/* Fills out debug messenger create info
 *
 * This function populates the necessary information to create a Vulkan debug
 * messenger.
 */
void VgeDevice::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional
}

/* Sets up the Vulkan debug messenger (if validation layers are enabled)
 *
 * This function creates the Vulkan debug messenger if validation layers are
 * enabled.
 */
void VgeDevice::setupDebugMessenger()
{
    if (!m_enableValidationLayers)
        return;
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);
    if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

/* Checks if the requested validation layers are supported
 *
 * This function checks if the required Vulkan validation layers are available.
 */
bool VgeDevice::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : m_validationLayers) {
        bool layerFound = false;

        for (const VkLayerProperties& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

/* Retrieves required Vulkan extensions, including GLFW and validation layers
 *
 * This function returns a list of required Vulkan extensions, including those
 * needed for GLFW and validation layers.
 */
std::vector<const char*> VgeDevice::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (m_enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

/* Checks if required GLFW extensions are supported by Vulkan
 *
 * This function checks if the required GLFW extensions are supported by the
 * Vulkan instance.
 */
void VgeDevice::hasGflwRequiredInstanceExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "available extensions:" << std::endl;
    std::unordered_set<std::string> available;
    for (const VkExtensionProperties& extension : extensions) {
        std::cout << "\t" << extension.extensionName << std::endl;
        available.insert(extension.extensionName);
    }

    std::cout << "required extensions:" << std::endl;
    std::vector<const char*> requiredExtensions = getRequiredExtensions();
    for (const char* const& required : requiredExtensions) {
        std::cout << "\t" << required << std::endl;
        if (available.find(required) == available.end()) {
            throw std::runtime_error("Missing required glfw extension");
        }
    }
}

/* Checks if a physical device supports the required Vulkan extensions
 *
 * This function checks if the physical device supports the required Vulkan
 * extensions by comparing the available extensions with those requested by the
 * application.
 */
bool VgeDevice::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &extensionCount,
        availableExtensions.data());

    std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

    for (const VkExtensionProperties& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

/* Finds queue families for a physical device that support graphics and
 * presentation
 *
 * This function finds the queue families for the physical device that support
 * both graphics and presentation operations.
 */
QueueFamilyIndices VgeDevice::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const VkQueueFamilyProperties& queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            // prevent type conversion
            indices.graphicsFamily = static_cast<uint32_t>(i);
            indices.graphicsFamilyHasValue = true;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            device,
            static_cast<uint32_t>(i),
            m_surface_,
            &presentSupport);
        if (queueFamily.queueCount > 0 && presentSupport) {
            // prevent type conversion
            indices.presentFamily = static_cast<uint32_t>(i);
            indices.presentFamilyHasValue = true;
        }
        if (indices.isComplete()) {
            break;
        }

        i++; // still type int on increment
    }

    return indices;
}

/* Queries swap chain support for a physical device
 *
 * This function queries the swap chain support details for a physical device,
 * including surface capabilities, formats, and present modes.
 */
SwapChainSupportDetails VgeDevice::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface_, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface_, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            device,
            m_surface_,
            &formatCount,
            details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface_, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device,
            m_surface_,
            &presentModeCount,
            details.presentModes.data());
    }
    return details;
}

/* Finds a supported image format for the physical device
 *
 * This function searches for a suitable image format supported by the physical
 * device based on the provided tiling and format feature flags.
 */
VkFormat VgeDevice::findSupportedFormat(
    const std::vector<VkFormat>& candidates,
    VkImageTiling tiling,
    VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (
            tiling == VK_IMAGE_TILING_OPTIMAL &&
            (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }
    throw std::runtime_error("failed to find supported format!");
}

/* Finds the memory properties for your GPU
 *
 * Queries the physical device for its memory properties and selects a memory
 * type that satisfies the given type filter and properties.
 */
uint32_t VgeDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

/* Create a memory storage buffer
 *
 * Creates a Vulkan buffer with the specified size and usage flags, allocates
 * memory for the buffer, and binds the memory to the buffer.
 */
void VgeDevice::createBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer,
    VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_device_, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device_, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_device_, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(m_device_, buffer, bufferMemory, 0);
}

/* Initialize buffer for a single command
 *
 * Allocates a command buffer for single-time command execution and begins
 * recording commands into it.
 */
VkCommandBuffer VgeDevice::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_device_, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

/* Execute the buffered command and cleanup the buffer
 *
 * Ends command recording, submits the command buffer to the graphics queue,
 * waits for the queue to finish processing, and then frees the command buffer.
 */
void VgeDevice::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_graphicsQueue_);

    vkFreeCommandBuffers(m_device_, m_commandPool, 1, &commandBuffer);
}

/* Copy source buffer to destination buffer and cleanup
 *
 * Copies data from the source buffer to the destination buffer using a
 * single-time command buffer and then cleans up the command buffer.
 */
void VgeDevice::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

/* Copy the buffer data to an image
 *
 * Copies data from a buffer to an image using a single-time command buffer and
 * specifies the region of the image to copy the buffer data into.
 */
void VgeDevice::copyBufferToImage(
    VkBuffer buffer,
    VkImage image,
    uint32_t width,
    uint32_t height,
    uint32_t layerCount)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = layerCount;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
    endSingleTimeCommands(commandBuffer);
}

/* Create a Vulkan image
 *
 * Creates a Vulkan image with the given create info, allocates memory for it,
 * and binds the memory to the image.
 */
void VgeDevice::createImageWithInfo(
    const VkImageCreateInfo& imageInfo,
    VkMemoryPropertyFlags properties,
    VkImage& image,
    VkDeviceMemory& imageMemory)
{
    if (vkCreateImage(m_device_, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_device_, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_device_, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    if (vkBindImageMemory(m_device_, image, imageMemory, 0) != VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }
}

/* Get the command pool
 *
 * Returns the Vulkan command pool associated with the device.
 */
VkCommandPool VgeDevice::getCommandPool()
{
    return m_commandPool;
}

/* Get the Vulkan device
 *
 * Returns the Vulkan logical device created for the application.
 */
VkDevice VgeDevice::getDevice()
{
    return m_device_;
}

/* Get the surface
 *
 * Returns the Vulkan surface associated with the device.
 */
VkSurfaceKHR VgeDevice::getSurface()
{
    return m_surface_;
}

/* Get the graphics queue
 *
 * Returns the Vulkan queue used for graphics operations.
 */
VkQueue VgeDevice::getGraphicsQueue()
{
    return m_graphicsQueue_;
}

/* Get the present queue
 *
 * Returns the Vulkan queue used for presenting images to the swap chain.
 */
VkQueue VgeDevice::getPresentQueue()
{
    return m_presentQueue_;
}

/* Get swap chain support details
 *
 * Queries the physical device for the supported swap chain capabilities,
 * formats, and present modes.
 */
SwapChainSupportDetails VgeDevice::getSwapChainSupport()
{
    return querySwapChainSupport(m_physicalDevice);
}

/* Find the queue families for the physical device
 *
 * Finds the queue families supported by the physical device that are suitable
 * for graphics and presentation operations.
 */
QueueFamilyIndices VgeDevice::findPhysicalQueueFamilies()
{
    return findQueueFamilies(m_physicalDevice);
}

/* Checks if both graphics and present families have been set.
 *
 * This function returns true if both the graphicsFamily and presentFamily
 * indices have valid values, indicating that the queue families required
 * for rendering and presentation are available for use.
 */
bool QueueFamilyIndices::isComplete()
{
    return graphicsFamilyHasValue && presentFamilyHasValue;
}

} // namespace vge

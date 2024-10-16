#include "vge_instance.hpp"
#include <GLFW/glfw3.h>
#include <cstring>
#include <iostream>
#include <unordered_set>

namespace vge {
VgeInstance::VgeInstance()
    : m_vgeValidationLayers{}
    , m_enableVLayers{ m_vgeValidationLayers.areVLayersEnabled() }
    , m_VLayerSupport{ m_vgeValidationLayers.areVLayersSupported() }
    , m_VLayers{ m_vgeValidationLayers.getVLayers() }
{
    // Prepatory functions before instance creation
    setRequiredExts();
    hasRequiredInstanceExts();

    createInstance();
    setupDebugMessenger();
}

VgeInstance::~VgeInstance()
{
    // Validation layers cleanup happens first
    if (m_enableVLayers) {
        destroyDebugMessenger(m_instance, m_debugMessenger, nullptr);
    }

    if (m_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}

void VgeInstance::setRequiredExts()
{
    uint32_t glfwExtCount = 0;
    const char** glfwExts;

    glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);

    m_requiredExts = std::vector<const char*>(glfwExts, glfwExts + glfwExtCount);

    if (m_enableVLayers) {
        m_requiredExts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
}

void VgeInstance::hasRequiredInstanceExts()
{
    uint32_t extCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
    std::vector<VkExtensionProperties> exts(extCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, exts.data());

    std::cout << "Available instance extensions:\n";
    std::unordered_set<std::string> available;
    for (const VkExtensionProperties& ext : exts) {
        std::cout << "\tInstance: " << ext.extensionName << '\n';
        available.insert(ext.extensionName);
    }

    std::cout << "Required instance extensions:\n";
    for (const char* const& required : m_requiredExts) {
        std::cout << "\tInstance: " << required << '\n';
        if (available.find(required) == available.end()) {
            throw std::runtime_error("Missing required glfw extension");
        }
    }
}

void VgeInstance::createInstance()
{
    // Check if validation layers are enabled/supported
    if (m_enableVLayers && !m_VLayerSupport) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    // Provides app and engine info the the Vulkan implementation
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,    // sType
        .pNext = nullptr,                               // pNext (no additional structures)
        .pApplicationName = "Vulkan Game Engine",       // pApplicationName
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0), // applicationVersion
        .pEngineName = "Vulkan Game Engine",            // pEngineName
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),      // engineVersion
        .apiVersion = VK_API_VERSION_1_0,               // apiVersion
    };

    // Provides details for the Vulkan Instance
    VkInstanceCreateInfo instanceCI = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // sType
        .pNext = nullptr,                                // pNext (set conditionally later)
        .flags = 0,                                      // flags
        .pApplicationInfo = &appInfo,                    // pApplicationInfo
        .enabledLayerCount = 0,         // enabledLayerCount (set conditionally later)
        .ppEnabledLayerNames = nullptr, // ppEnabledLayerNames (set conditionally later)
        .enabledExtensionCount =
            static_cast<uint32_t>(m_requiredExts.size()), // enabledExtensionCount
        .ppEnabledExtensionNames = m_requiredExts.data(), // ppEnabledExtensionNames
    };

    // Sets instance info depending if validation layers are available
    if (m_enableVLayers) {
        instanceCI.enabledLayerCount = static_cast<uint32_t>(m_VLayers.size());
        instanceCI.ppEnabledLayerNames = m_VLayers.data();

        populateDebugMessenger(m_debugCI);
        instanceCI.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&m_debugCI;
    }

    // Stores the instance inside m_instance with the struct creation info
    if (vkCreateInstance(&instanceCI, nullptr, &m_instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

void VgeInstance::setupDebugMessenger()
{
    if (!m_enableVLayers)
        return;

    if (createDebugMessenger(m_instance, &m_debugCI, nullptr, &m_debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void VgeInstance::populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& debugCI)
{
    debugCI = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr, // Optional
    };
}

// TODO: Add color codes for messages?
VKAPI_ATTR VkBool32 VKAPI_CALL VgeInstance::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    [[maybe_unused]] void* pUserData)
{
    std::string formattedMessage;

    // Check the message type and construct appropriate messages
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        formattedMessage = "General: ";
    }
    else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        formattedMessage = "Validation: ";
    }
    else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        formattedMessage = "Performance: ";
    }

    // Append the severity level
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        formattedMessage += "Error: ";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        formattedMessage += "Warning: ";
    }
    else {
        formattedMessage += "Info: ";
    }

    // Append and output the actual message from pCallbackData
    formattedMessage += pCallbackData->pMessage;
    std::cerr << formattedMessage << std::endl;

    return VK_FALSE;
}

VkResult VgeInstance::createDebugMessenger(
    const VkInstance& instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pDebugCInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pDebugCInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VgeInstance::destroyDebugMessenger(
    const VkInstance& instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

VkInstance VgeInstance::getInstance() const
{
    return m_instance;
}

} // namespace vge

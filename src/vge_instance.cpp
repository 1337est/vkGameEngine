#include "vge_instance.hpp"
#include <GLFW/glfw3.h>
#include <cstring>
#include <iostream>
#include <unordered_set>

namespace vge {
VgeInstance::VgeInstance()
#ifdef NDEBUG
    : m_enableValidationLayers{ false }
#else
    : m_enableValidationLayers{ true }
#endif
{
    setRequiredExtensions();
    checkValidationLayerSupport();
    createInstance();
    setupDebugMessenger();
}

VgeInstance::~VgeInstance()
{
    // Validation layers cleanup happens first
    if (m_enableValidationLayers) {
        destroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }

    if (m_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}

void VgeInstance::createInstance()
{
    // Check if validation layers are enabled/supported
    if (m_enableValidationLayers && !m_validationLayerSupported) {
        throw std::runtime_error(
            "validation layers requested, but not available!");
    }

    // Provides app and engine info the the Vulkan implementation
    VkApplicationInfo appInfo = {
        VK_STRUCTURE_TYPE_APPLICATION_INFO, // sType
        nullptr,                            // pNext (no additional structures)
        "Vulkan Game Engine",               // pApplicationName
        VK_MAKE_VERSION(1, 0, 0),           // applicationVersion
        "Vulkan Game Engine",               // pEngineName
        VK_MAKE_VERSION(1, 0, 0),           // engineVersion
        VK_API_VERSION_1_0                  // apiVersion
    };

    // Provides details for the Vulkan Instance
    VkInstanceCreateInfo instanceCreateInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // sType
        nullptr,  // pNext (set conditionally later)
        0,        // flags
        &appInfo, // pApplicationInfo
        0,        // enabledLayerCount (set conditionally later)
        nullptr,  // ppEnabledLayerNames (set conditionally later)
        static_cast<uint32_t>(
            m_requiredExtensions.size()), // enabledExtensionCount
        m_requiredExtensions.data()       // ppEnabledExtensionNames
    };

    // Sets instance info depending if validation layers are available
    if (m_enableValidationLayers) {
        instanceCreateInfo.enabledLayerCount =
            static_cast<uint32_t>(m_validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = m_validationLayers.data();

        populateDebugMessengerCreateInfo(m_debugCreateInfo);
        instanceCreateInfo.pNext =
            (VkDebugUtilsMessengerCreateInfoEXT*)&m_debugCreateInfo;
    }

    // Stores the instance inside m_instance with the struct creation info
    if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }

    hasGlfwRequiredInstanceExtensions();
}

void VgeInstance::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    m_validationLayerSupported = true;

    for (const char* layerName : m_validationLayers) {
        bool layerFound = false;

        for (const VkLayerProperties& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            m_validationLayerSupported = false;
            return;
        }
    }
}

void VgeInstance::setRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    m_requiredExtensions = std::vector<const char*>(
        glfwExtensions,
        glfwExtensions + glfwExtensionCount);

    if (m_enableValidationLayers) {
        m_requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
}

void VgeInstance::hasGlfwRequiredInstanceExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(
        nullptr,
        &extensionCount,
        extensions.data());

    std::cout << "Available instance extensions:\n";
    std::unordered_set<std::string> available;
    for (const VkExtensionProperties& extension : extensions) {
        std::cout << "\tInstance: " << extension.extensionName << '\n';
        available.insert(extension.extensionName);
    }

    std::cout << "Required instance extensions:\n";
    for (const char* const& required : m_requiredExtensions) {
        std::cout << "\tInstance: " << required << '\n';
        if (available.find(required) == available.end()) {
            throw std::runtime_error("Missing required glfw extension");
        }
    }
}

void VgeInstance::setupDebugMessenger()
{
    if (!m_enableValidationLayers)
        return;

    if (createDebugUtilsMessengerEXT(
            m_instance,
            &m_debugCreateInfo,
            nullptr,
            &m_debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void VgeInstance::populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo)
{
    debugCreateInfo = {};
    debugCreateInfo.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debugCallback;
    debugCreateInfo.pUserData = nullptr; // Optional
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
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
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

VkResult VgeInstance::createDebugUtilsMessengerEXT(
    const VkInstance& instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT func =
        (PFN_vkCreateDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VgeInstance::destroyDebugUtilsMessengerEXT(
    const VkInstance& instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func =
        (PFN_vkDestroyDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

VkInstance VgeInstance::getInstance() const
{
    return m_instance;
}

bool VgeInstance::areValidationLayersEnabled() const
{
    return m_enableValidationLayers;
}

const std::vector<const char*>& VgeInstance::getValidationLayers() const
{
    return m_validationLayers;
}

} // namespace vge

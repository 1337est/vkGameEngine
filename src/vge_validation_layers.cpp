// headers
#include "vge_validation_layers.hpp"

// libs
#include <vulkan/vulkan.h>

// std
#include <cstring>
#include <iostream>
#include <vulkan/vulkan_core.h>

namespace vge
{

// Initializes validation layers based on build configuration.
VgeValidationLayers::VgeValidationLayers()
#ifdef NDEBUG
    : m_enableValidationLayers{ false }
#else
    : m_enableValidationLayers{ true }
#endif
{

    std::cout << "Creating VgeValidationLayers. Validation layers enabled: "
              << (m_enableValidationLayers ? "true" : "false") << std::endl;
}

VgeValidationLayers::~VgeValidationLayers()
{

    std::cout << "Destroying VgeValidationLayers." << std::endl;

    cleanup();
}

void VgeValidationLayers::setInstance(VkInstance instance)
{
    m_instance = instance;
    setupDebugMessenger();
}

/* Sets up the Vulkan debug messenger (if validation layers are enabled)
 *
 * This function creates the Vulkan debug messenger if validation layers are
 * enabled.
 */
void VgeValidationLayers::setupDebugMessenger()
{
    if (!m_enableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (createDebugUtilsMessengerEXT(
            m_instance,
            &createInfo,
            nullptr,
            &m_debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

/* Fills out debug messenger create info
 *
 * This function populates the necessary information to create a Vulkan debug
 * messenger.
 */
void VgeValidationLayers::populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional
}

/* Callback function for Vulkan debug messages.
 *
 * This function is called by Vulkan whenever a validation message needs to be
 * logged, It writes the validation message to std::cerr and returns VK_FALSE to
 * indicate that the call should not be aborted.
 */
VKAPI_ATTR VkBool32 VKAPI_CALL VgeValidationLayers::debugCallback(
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
VkResult VgeValidationLayers::createDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT func =
        (PFN_vkCreateDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

/* Checks if the requested validation layers are supported
 *
 * This function checks if the required Vulkan validation layers are
 * available.
 */
bool VgeValidationLayers::checkValidationLayerSupport() const
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : m_validationLayers)
    {
        bool layerFound = false;

        for (const VkLayerProperties& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

void VgeValidationLayers::cleanup()
{
    if (m_debugMessenger != VK_NULL_HANDLE)
    {
        destroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
        m_debugMessenger = VK_NULL_HANDLE;
    }
}

/* Destroy Vulkan debug messenger
 *
 * This function destroys a previously created Vulkan debug messenger using the
 * `vkDestroyDebugUtilsMessengerEXT` function. It is retrieved similarly to
 * `CreateDebugUtilsMessengerEXT` using `vkGetInstanceProcAddr`.
 */
void VgeValidationLayers::destroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func =
        (PFN_vkDestroyDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}
} // namespace vge

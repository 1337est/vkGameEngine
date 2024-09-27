// headers
#include "vge_instance.hpp"
#include "vge_validation_layers.hpp"

// libs
#include <GLFW/glfw3.h>

// std
#include <iostream>
#include <unordered_set>

namespace vge
{

/* Initializes a vulkan instance
 *
 * Calls `createInstance` to set up the Vulkan instance.
 */
VgeInstance::VgeInstance()
{
    createInstance();
    m_validationLayers.setInstance(m_instance);
}

/* Cleans up the Vulkan instance
 *
 * When the instance goes out of scope, it destroys and releases the allocated
 * resources. It then sets the intance handle back to VK_NULL_HANDLE to avoid
 * dangling pointers.
 */
VgeInstance::~VgeInstance()
{
    // Validation layers cleanup happens first
    m_validationLayers.cleanup();

    if (m_instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}

/* Create a Vulkan instance
 *
 * Sets up the `VkApplicationInfo` with application and engine information,
 * prepares a `VkInstanceCreateInfo` structure with required extensions, and
 * calls `vkCreateInstance`. If instance creation fails, it throws a runtime
 * error. The method also verifies required GLFW extensions by calling
 * `hasGlfwRequiredInstanceExtensions`.
 */
void VgeInstance::createInstance()
{
    if (m_validationLayers.areValidationLayersEnabled() &&
        !m_validationLayers.checkValidationLayerSupport())
    {
        throw std::runtime_error(
            "validation layers requested, but not available!");
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

    // Configure enabled layers
    if (m_validationLayers.areValidationLayersEnabled())
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(
            m_validationLayers.getValidationLayers().size());
        createInfo.ppEnabledLayerNames =
            m_validationLayers.getValidationLayers().data();

        m_validationLayers.populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext =
            (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }

    hasGlfwRequiredInstanceExtensions();
}

/* Retrieves the necessary GLFW extensions for the Vulkan instance
 *
 * Calls `glfwGetRequiredInstanceExtensions` to obtain the names of the required
 * extensions and returns them as a vector of const char*, allowing other parts
 * of the code to access the necessary extensions for Vulkan initialization.
 */
std::vector<const char*> VgeInstance::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(
        glfwExtensions,
        glfwExtensions + glfwExtensionCount);

    if (m_validationLayers.areValidationLayersEnabled())
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

/* Checks if the required GLFW extensions are supported by the Vulkan instance
 *
 * Retrieves the list of available extensions and compares them with the
 * required extensions obtained from `getRequiredExtensions`. If any necessary
 * extensions are missing it throws a runtime error.
 */
void VgeInstance::hasGlfwRequiredInstanceExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(
        nullptr,
        &extensionCount,
        extensions.data());

    std::cout << "available extensions:" << std::endl;
    std::unordered_set<std::string> available;
    for (const VkExtensionProperties& extension : extensions)
    {
        std::cout << "\t" << extension.extensionName << std::endl;
        available.insert(extension.extensionName);
    }

    std::cout << "required extensions:" << std::endl;
    std::vector<const char*> requiredExtensions = getRequiredExtensions();
    for (const char* const& required : requiredExtensions)
    {
        std::cout << "\t" << required << std::endl;
        if (available.find(required) == available.end())
        {
            throw std::runtime_error("Missing required glfw extension");
        }
    }
}

} // namespace vge

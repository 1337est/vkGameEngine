#include "vge_instance.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_set>

namespace vge
{
VgeInstance::VgeInstance()
{
    std::cout << "START: VgeInstance Constructor\n";
    createInstance();
    std::cout << "\tInstance created\n";
    m_validationLayers.setupDebugMessenger(m_instance);
    std::cout << "\tDebug messenger setup completed\n";
    std::cout << "END: VgeInstance Constructor\n\n";
}

VgeInstance::~VgeInstance()
{
    std::cout << "START: VgeInstance Destructor\n";
    // Validation layers cleanup happens first
    m_validationLayers.cleanup(m_instance);
    std::cout << "\tVulkan Debug Messenger destroyed.\n";

    if (m_instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
        std::cout << "\tVulkan Instance destroyed.\n";
    }
    std::cout << "END: VgeInstance Destructor\n\n";
}

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

void VgeInstance::hasGlfwRequiredInstanceExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(
        nullptr,
        &extensionCount,
        extensions.data());

    std::cout << "available extensions:\n";
    std::unordered_set<std::string> available;
    for (const VkExtensionProperties& extension : extensions)
    {
        std::cout << "\t" << extension.extensionName << '\n';
        available.insert(extension.extensionName);
    }

    std::cout << "required extensions:\n";
    std::vector<const char*> requiredExtensions = getRequiredExtensions();
    for (const char* const& required : requiredExtensions)
    {
        std::cout << "\t" << required << '\n';
        if (available.find(required) == available.end())
        {
            throw std::runtime_error("Missing required glfw extension");
        }
    }
}

VkInstance VgeInstance::getInstance() const
{
    return m_instance;
}

} // namespace vge

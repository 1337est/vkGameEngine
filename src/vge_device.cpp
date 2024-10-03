#include "vge_device.hpp"
#include "vge_queue_families.hpp"
#include <iostream>
#include <set>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge
{

VgeDevice::VgeDevice(
    const VkInstance& instance,
    VkSurfaceKHR surface,
    bool enableValidationLayers,
    const std::vector<const char*> validationLayers)
{
    std::cout << "START: VgeDevice Constructor\n";
    pickPhysicalDevice(instance, surface);
    std::cout << "\tPhysical device selected.\n";
    createLogicalDevice(surface, enableValidationLayers, validationLayers);
    std::cout << "\tLogical device created.\n";
    std::cout << "END: VgeDevice Constructor\n\n";
}

VgeDevice::~VgeDevice()
{
    std::cout << "START: VgeDevice Destructor\n";
    vkDestroyDevice(m_logicalDevice, nullptr);
    std::cout << "\tLogical device destroyed.\n";
    std::cout << "END: VgeDevice Destructor\n\n";
}

void VgeDevice::pickPhysicalDevice(
    const VkInstance& instance,
    VkSurfaceKHR surface)
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    if (physicalDeviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::cout << "Device count: " << physicalDeviceCount << std::endl;

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(
        instance,
        &physicalDeviceCount,
        physicalDevices.data());

    for (const VkPhysicalDevice& physicalDevice : physicalDevices)
    {
        if (isDeviceSuitable(physicalDevice, surface))
        {
            m_physicalDevice = physicalDevice;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    vkGetPhysicalDeviceProperties(
        m_physicalDevice,
        &m_physicalDeviceProperties);
    std::cout << "physical device: " << m_physicalDeviceProperties.deviceName
              << std::endl;
}

bool VgeDevice::isDeviceSuitable(
    const VkPhysicalDevice& physicalDevice,
    VkSurfaceKHR surface)
{
    VgeQueueFamilies queueFamilies(physicalDevice, surface);
    bool extensionsSupported = checkDeviceExtensionSupport(physicalDevice);

    return queueFamilies.isComplete() && extensionsSupported;
}

bool VgeDevice::checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(
        physicalDevice,
        nullptr,
        &extensionCount,
        nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        physicalDevice,
        nullptr,
        &extensionCount,
        availableExtensions.data());

    std::set<std::string> requiredExtensions(
        m_deviceExtensions.begin(),
        m_deviceExtensions.end());

    for (const VkExtensionProperties& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

void VgeDevice::createLogicalDevice(
    VkSurfaceKHR surface,
    bool enableValidationLayers,
    std::vector<const char*> validationLayers)
{
    VgeQueueFamilies queueFamily(m_physicalDevice, surface);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { queueFamily.getGraphicsFamily(),
                                               queueFamily.getPresentFamily() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE; // Enable any required features

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount =
        static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount =
        static_cast<uint32_t>(m_deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount =
            static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(
            m_physicalDevice,
            &createInfo,
            nullptr,
            &m_logicalDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(
        m_logicalDevice,
        queueFamily.getGraphicsFamily(),
        0,
        &m_graphicsQueue);
    vkGetDeviceQueue(
        m_logicalDevice,
        queueFamily.getPresentFamily(),
        0,
        &m_presentQueue);
}

VkPhysicalDevice VgeDevice::getPhysicalDevice() const
{
    return m_physicalDevice;
}

VkDevice VgeDevice::getLogicalDevice() const
{
    return m_logicalDevice;
}

} // namespace vge

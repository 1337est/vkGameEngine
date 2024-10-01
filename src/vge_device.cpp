#include "vge_device.hpp"
#include "vge_queue_families.hpp"
#include <iostream>
#include <set>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge
{

/* Selects a physical device
 *
 * Initializes the `VgeDevice` object and calls the `pickPhysicalDevice`
 * function to select a suitable physical GPU from the Vulkan instance
 */
VgeDevice::VgeDevice(
    const VkInstance& instance,
    VkSurfaceKHR surface,
    VgeValidationLayers& validationLayers)
    : m_enableValidationLayers(validationLayers.areValidationLayersEnabled())
    , m_validationLayers(validationLayers.getValidationLayers())
{
    pickPhysicalDevice(instance, surface);
    createLogicalDevice(surface);
}

// TODO: Description
VgeDevice::~VgeDevice()
{
    vkDestroyDevice(m_logicalDevice, nullptr);
}

/* Selects a physical GPU for the Vulkan application
 *
 * This function enumerates available physical devices associated with
 * the provided Vulkan instance and selects one that meets the
 * requirements for the Vulkan application. It throws an exception
 * if no suitable device is found.
 */
void VgeDevice::pickPhysicalDevice(
    const VkInstance& instance,
    VkSurfaceKHR surface)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::cout << "Device count: " << deviceCount << std::endl;

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const VkPhysicalDevice& device : devices)
    {
        if (isDeviceSuitable(device, surface))
        {
            m_physicalDevice = device;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    vkGetPhysicalDeviceProperties(m_physicalDevice, &m_deviceProperties);
    std::cout << "physical device: " << m_deviceProperties.deviceName
              << std::endl;
}

/* Checks if a physical device is suitable for the Vulkan application
 *
 * This function evaluates the specified physical device to determine
 * if it supports the required features for the Vulkan application.
 * It uses the findQueueFamilies method to check for the necessary queue
 * families.
 */
bool VgeDevice::isDeviceSuitable(
    const VkPhysicalDevice& device,
    VkSurfaceKHR surface)
{
    VgeQueueFamilies queueFamilies(device, surface);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    return queueFamilies.isComplete() && extensionsSupported;
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
    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &extensionCount,
        nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        device,
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

/* Creates a logical device for the selected physical device
 *
 * This function uses the queue families found for the physical device
 * to create a logical device that can interact with the GPU.
 */
void VgeDevice::createLogicalDevice(VkSurfaceKHR surface)
{
    VgeQueueFamilies queueFamilies(m_physicalDevice, surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        queueFamilies.getGraphicsFamily(),
        queueFamilies.getPresentFamily()
    };

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

    if (m_enableValidationLayers)
    {
        createInfo.enabledLayerCount =
            static_cast<uint32_t>(m_validationLayers.size());
        createInfo.ppEnabledLayerNames = m_validationLayers.data();
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
        queueFamilies.getGraphicsFamily(),
        0,
        &m_graphicsQueue);

    vkGetDeviceQueue(
        m_logicalDevice,
        queueFamilies.getPresentFamily(),
        0,
        &m_presentQueue);
}

// Retrieves the selected physical device from `pickPhysicalDevice`
VkPhysicalDevice VgeDevice::getPhysicalDevice() const
{
    return m_physicalDevice;
}

VkDevice VgeDevice::getLogicalDevice() const
{
    return m_logicalDevice;
}

VkQueue VgeDevice::getGraphicsQueue() const
{
    return m_graphicsQueue;
}

VkQueue VgeDevice::getPresentQueue() const
{
    return m_presentQueue;
}

} // namespace vge

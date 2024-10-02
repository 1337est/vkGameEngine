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
    VgeValidationLayers& validationLayers)
    : m_enableValidationLayers(validationLayers.areValidationLayersEnabled())
    , m_validationLayers(validationLayers.getValidationLayers())
{
    std::cout << "VgeDevice Constructor: Starting device creation process.\n";
    std::cout << "VgeDevice Constructor: Picking physical device and creating "
                 "logical device.\n";
    pickPhysicalDevice(instance, surface);
    createLogicalDevice(surface);
    std::cout << "VgeDevice Constructor: Physical device selected and logical "
                 "device created.\n";
    std::cout << "VgeDevice Constructor: Device creation process completed.\n";
}

VgeDevice::~VgeDevice()
{
    std::cout << "VgeDevice Destructor: Starting logical device destruction.\n";
    vkDestroyDevice(m_logicalDevice, nullptr);
    std::cout << "VgeDevice Destructor: Logical device destroyed.\n";
    std::cout << "VgeDevice Destructor: Device destruction completed.\n";
}

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

bool VgeDevice::isDeviceSuitable(
    const VkPhysicalDevice& device,
    VkSurfaceKHR surface)
{
    VgeQueueFamilies queueFamilies(device, surface);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    return queueFamilies.isComplete() && extensionsSupported;
}

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

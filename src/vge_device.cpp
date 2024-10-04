#include "vge_device.hpp"
#include <iostream>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {

VgeDevice::VgeDevice(
    const VkInstance& instance,
    VkSurfaceKHR surface,
    bool enableValidationLayers,
    const std::vector<const char*> validationLayers)
{
    pickPhysicalDevice(instance, surface);
    createLogicalDevice(enableValidationLayers, validationLayers);
}

VgeDevice::~VgeDevice()
{
    vkDestroyDevice(m_logicalDevice, nullptr);
}

void VgeDevice::pickPhysicalDevice(
    const VkInstance& instance,
    VkSurfaceKHR surface)
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    if (physicalDeviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::cout << "Device count: " << physicalDeviceCount << std::endl;

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(
        instance,
        &physicalDeviceCount,
        physicalDevices.data());

    for (const VkPhysicalDevice& physicalDevice : physicalDevices) {
        if (isDeviceSuitable(physicalDevice, surface)) {
            m_physicalDevice = physicalDevice;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE) {
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
    findQueueFamilies(physicalDevice, surface);
    bool extensionsSupported = checkDeviceExtensionSupport(physicalDevice);

    return isComplete() && extensionsSupported;
}

void VgeDevice::findQueueFamilies(
    const VkPhysicalDevice& physicalDevice,
    VkSurfaceKHR surface)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice,
        &queueFamilyCount,
        nullptr);

    if (queueFamilyCount == 0) {
        throw std::runtime_error("Failed to find queue families.");
    }

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice,
        &queueFamilyCount,
        queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

        // check if queue support graphics operations
        if (queueFamily.queueCount > 0 &&
            queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            m_graphicsFamily = i;
            m_graphicsFamilyHasValue = true;
        }

        // check if queue supports presentation to the surface
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            physicalDevice,
            i,
            surface,
            &presentSupport);
        if (queueFamily.queueCount > 0 && presentSupport) {
            m_presentFamily = i;
            m_presentFamilyHasValue = true;
        }

        if (isComplete()) {
            break;
        }
    }

    // throw error if no families are found
    if (!m_graphicsFamilyHasValue) {
        throw std::runtime_error("Failed to find a graphics queue family.");
    }
    if (!m_presentFamilyHasValue) {
        throw std::runtime_error("Failed to find a presentation queue family.");
    }
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

    std::cout << "available device extensions:\n";
    std::unordered_set<std::string> available;
    for (const VkExtensionProperties& extension : availableExtensions) {
        std::cout << "\t" << extension.extensionName << '\n';
        available.insert(extension.extensionName);
    }

    std::cout << "required device extensions:\n";
    std::set<std::string> requiredExtensions(
        m_deviceExtensions.begin(),
        m_deviceExtensions.end());
    for (const std::string& required : requiredExtensions) {
        std::cout << "\t" << required;
        if (available.find(required) != available.end()) {
            std::cout << " (FOUND)\n";
        }
        else {
            std::cout << " (MISSING)\n";
        }
    }

    for (const VkExtensionProperties& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

void VgeDevice::createLogicalDevice(
    bool enableValidationLayers,
    std::vector<const char*> validationLayers)
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { getGraphicsFamily(),
                                               getPresentFamily() };

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

    if (enableValidationLayers) {
        createInfo.enabledLayerCount =
            static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(
            m_physicalDevice,
            &createInfo,
            nullptr,
            &m_logicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_logicalDevice, getGraphicsFamily(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_logicalDevice, getPresentFamily(), 0, &m_presentQueue);
}

bool VgeDevice::isComplete() const
{
    return m_graphicsFamilyHasValue && m_presentFamilyHasValue;
}

uint32_t VgeDevice::getGraphicsFamily() const
{
    return m_graphicsFamily;
}

uint32_t VgeDevice::getPresentFamily() const
{
    return m_presentFamily;
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

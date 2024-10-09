#include "vge_device.hpp"
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {

VgeDevice::VgeDevice(
    const VkInstance& instance,
    VkSurfaceKHR surface,
    bool enableVLayers,
    const std::vector<const char*> vLayers)
{
    pickPDevice(instance, surface);
    createLDevice(enableVLayers, vLayers);
}

VgeDevice::~VgeDevice()
{
    vkDestroyDevice(m_lDevice, nullptr);
}

void VgeDevice::pickPDevice(const VkInstance& instance, VkSurfaceKHR surface)
{
    uint32_t pDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &pDeviceCount, nullptr);
    if (pDeviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::cout << "Device count: " << pDeviceCount << std::endl;

    std::vector<VkPhysicalDevice> pDevices(pDeviceCount);
    vkEnumeratePhysicalDevices(instance, &pDeviceCount, pDevices.data());

    for (const VkPhysicalDevice& pDevice : pDevices) {
        if (isPDeviceSuitable(pDevice, surface)) {
            m_pDevice = pDevice; // set physical device var
            break;
        }
    }

    if (m_pDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    vkGetPhysicalDeviceProperties(m_pDevice, &m_pDeviceProps);
    std::cout << "physical device: " << m_pDeviceProps.deviceName << std::endl;
}

bool VgeDevice::isPDeviceSuitable(const VkPhysicalDevice& pDevice, VkSurfaceKHR surface)
{
    findQueueFamilies(pDevice, surface);
    bool extsSupported = checkDeviceExts(pDevice);

    return isComplete() && extsSupported;
}

void VgeDevice::findQueueFamilies(const VkPhysicalDevice& pDevice, VkSurfaceKHR surface)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, nullptr);

    if (queueFamilyCount == 0) {
        throw std::runtime_error("Failed to find queue families.");
    }

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

        // check if queue support graphics operations
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            m_graphicsFamily = i;
            m_graphicsFamilyHasValue = true;
        }

        // check if queue supports presentation to the surface
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(pDevice, i, surface, &presentSupport);
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

bool VgeDevice::checkDeviceExts(VkPhysicalDevice pDevice)
{
    uint32_t extCount;
    vkEnumerateDeviceExtensionProperties(pDevice, nullptr, &extCount, nullptr);

    std::vector<VkExtensionProperties> exts(extCount);
    vkEnumerateDeviceExtensionProperties(pDevice, nullptr, &extCount, exts.data());

    std::cout << "Available device extensions:\n";
    std::unordered_set<std::string> available;
    for (const VkExtensionProperties& ext : exts) {
        std::cout << "\tDevice: " << ext.extensionName << '\n';
        available.insert(ext.extensionName);
    }

    std::cout << "Required device extensions:\n";
    bool allExtsSupported = true;
    std::vector<const char*> missingExts;

    for (const char* const& required : m_requiredExts) {
        std::cout << "\tDevice: " << required;
        if (available.find(required) != available.end()) {
            std::cout << " (FOUND)\n";
        }
        else {
            std::cout << " (MISSING)\n";
            missingExts.push_back(required);
            allExtsSupported = false;
            throw std::runtime_error("Missing required device extensions");
        }
    }
    return allExtsSupported;
}

void VgeDevice::createLDevice(bool enableVLayers, std::vector<const char*> vLayers)
{
    std::vector<VkDeviceQueueCreateInfo> queueCIVector;
    std::set<uint32_t> uniqueQueueFamilies = { m_graphicsFamily, m_presentFamily };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCI = {};
        queueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCI.queueFamilyIndex = queueFamily;
        queueCI.queueCount = 1;
        queueCI.pQueuePriorities = &queuePriority;
        queueCIVector.push_back(queueCI);
    }

    VkPhysicalDeviceFeatures pDeviceFeatures = {};
    pDeviceFeatures.samplerAnisotropy = VK_TRUE; // Enable any required features

    VkDeviceCreateInfo deviceCI = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                    nullptr,
                                    {},
                                    static_cast<uint32_t>(queueCIVector.size()),
                                    queueCIVector.data(),
                                    enableVLayers ? static_cast<uint32_t>(vLayers.size()) : 0,
                                    enableVLayers ? vLayers.data() : nullptr,
                                    static_cast<uint32_t>(m_requiredExts.size()),
                                    m_requiredExts.data(),
                                    &pDeviceFeatures };

    if (vkCreateDevice(m_pDevice, &deviceCI, nullptr, &m_lDevice) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_lDevice, m_graphicsFamily, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_lDevice, m_presentFamily, 0, &m_presentQueue);
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

VkPhysicalDevice VgeDevice::getPDevice() const
{
    return m_pDevice;
}

VkDevice VgeDevice::getLDevice() const
{
    return m_lDevice;
}

} // namespace vge

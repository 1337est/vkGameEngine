#pragma once

// hdrs
#include "vge_validation_layers.hpp"
// libs
#include <vulkan/vulkan_core.h>
// stds
#include <vector>

namespace vge
{

class VgeDevice
{
public:
    VgeDevice(
        const VkInstance& instance,
        VgeValidationLayers& validationLayers);
    ~VgeDevice();

    // Not copyable or movable
    VgeDevice(const VgeDevice&) = delete;
    VgeDevice& operator=(const VgeDevice&) = delete;
    VgeDevice(VgeDevice&&) = delete;
    VgeDevice& operator=(VgeDevice&&) = delete;

    VkPhysicalDevice getPhysicalDevice() const;
    VkDevice getLogicalDevice() const;

private:
    void pickPhysicalDevice(const VkInstance& instance);
    void createLogicalDevice();
    bool isDeviceSuitable(const VkPhysicalDevice& device);

    // Device handles
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_logicalDevice = VK_NULL_HANDLE;

    // Queue handles
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;

    // Properties and features
    VkPhysicalDeviceProperties m_deviceProperties;

    const std::vector<const char*> m_deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    bool m_enableValidationLayers = false;
    std::vector<const char*> m_validationLayers;
};
} // namespace vge

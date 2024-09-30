#pragma once

// libs
#include <vulkan/vulkan_core.h>

namespace vge
{

struct QueueFamilyIndices
{
    uint32_t graphicsFamily{};
    bool graphicsFamilyHasValue = false;

    bool isComplete();
};

class VgeDevicePhysical
{
public:
    VgeDevicePhysical(const VkInstance& instance);
    ~VgeDevicePhysical();

    // Not copyable or movable
    VgeDevicePhysical(const VgeDevicePhysical&) = delete;
    VgeDevicePhysical& operator=(const VgeDevicePhysical&) = delete;
    VgeDevicePhysical(VgeDevicePhysical&&) = delete;
    VgeDevicePhysical& operator=(VgeDevicePhysical&&) = delete;

    VkPhysicalDevice getPhysicalDevice() const;

private:
    void pickPhysicalDevice(const VkInstance& instance);

    // helper functions
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);

    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties m_deviceProperties;
};
} // namespace vge

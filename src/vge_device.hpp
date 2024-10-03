#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge
{

class VgeDevice
{
public:
    VgeDevice(
        const VkInstance& instance,
        VkSurfaceKHR surface,
        bool enableValidationLayers,
        const std::vector<const char*> validationLayers);
    ~VgeDevice();

    // Not copyable or movable
    VgeDevice(const VgeDevice&) = delete;
    VgeDevice& operator=(const VgeDevice&) = delete;
    VgeDevice(VgeDevice&&) = delete;
    VgeDevice& operator=(VgeDevice&&) = delete;

    VkPhysicalDevice getPhysicalDevice() const;
    VkDevice getLogicalDevice() const;

private:
    void pickPhysicalDevice(const VkInstance& instance, VkSurfaceKHR surface);
    void createLogicalDevice(
        VkSurfaceKHR surface,
        bool enableValidationLayers,
        std::vector<const char*> validationLayers);
    bool isDeviceSuitable(
        const VkPhysicalDevice& physicalDevice,
        VkSurfaceKHR surface);
    bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);

    // Device handles
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_logicalDevice = VK_NULL_HANDLE;

    // Queue handles
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;

    // Properties and features
    VkPhysicalDeviceProperties m_physicalDeviceProperties;

    const std::vector<const char*> m_deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};
} // namespace vge

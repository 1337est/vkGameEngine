#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {

class VgeDevice {
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
    bool isComplete() const;
    uint32_t getGraphicsFamily() const;
    uint32_t getPresentFamily() const;

private:
    void pickPhysicalDevice(const VkInstance& instance, VkSurfaceKHR surface);
    bool isDeviceSuitable(
        const VkPhysicalDevice& physicalDevice,
        VkSurfaceKHR surface);
    void findQueueFamilies(
        const VkPhysicalDevice& physicalDevice,
        VkSurfaceKHR surface);
    bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
    void createLogicalDevice(
        bool enableValidationLayers,
        std::vector<const char*> validationLayers);

    // Device handles
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_logicalDevice = VK_NULL_HANDLE;

    // Queue handles
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    uint32_t m_graphicsFamily = UINT32_MAX;
    uint32_t m_presentFamily = UINT32_MAX;
    bool m_graphicsFamilyHasValue = false;
    bool m_presentFamilyHasValue = false;

    // Properties and features
    VkPhysicalDeviceProperties m_physicalDeviceProperties;

    // Extensions
    const std::vector<const char*> m_requiredExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};
} // namespace vge

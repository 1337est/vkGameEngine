#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {

class VgeDevice {
public:
    VgeDevice(
        const VkInstance& instance,
        VkSurfaceKHR surface,
        bool enableVLayers,
        const std::vector<const char*> vLayers);
    ~VgeDevice();

    // Not copyable or movable
    VgeDevice(const VgeDevice&) = delete;
    VgeDevice& operator=(const VgeDevice&) = delete;
    VgeDevice(VgeDevice&&) = delete;
    VgeDevice& operator=(VgeDevice&&) = delete;

    VkPhysicalDevice getPDevice() const;
    VkDevice getLDevice() const;
    bool isComplete() const;
    uint32_t getGraphicsFamily() const;
    uint32_t getPresentFamily() const;

private:
    void pickPDevice(const VkInstance& instance, VkSurfaceKHR surface);
    bool isPDeviceSuitable(const VkPhysicalDevice& pDevice, VkSurfaceKHR surface);
    void findQueueFamilies(const VkPhysicalDevice& pDevice, VkSurfaceKHR surface);
    bool checkDeviceExts(VkPhysicalDevice pDevice);
    void createLDevice(bool enableVLayers, std::vector<const char*> vLayers);

    // Device handles
    VkPhysicalDevice m_pDevice = VK_NULL_HANDLE;
    VkDevice m_lDevice = VK_NULL_HANDLE;

    // Queue handles
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    uint32_t m_graphicsFamily = UINT32_MAX;
    uint32_t m_presentFamily = UINT32_MAX;
    bool m_graphicsFamilyHasValue = false;
    bool m_presentFamilyHasValue = false;

    // Properties and features
    VkPhysicalDeviceProperties m_pDeviceProps;

    // Extensions
    const std::vector<const char*> m_requiredExts = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};
} // namespace vge

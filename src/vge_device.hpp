#pragma once
#include "vge_instance.hpp"
#include "vge_surface.hpp"
#include "vge_validation_layers.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {

class VgeDevice {
public:
    VgeDevice(
        VgeInstance& vgeInstance,
        VgeSurface& vgeSurface,
        VgeValidationLayers& vgeValidationLayers);
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
    VkQueue getGraphicsQueue() const;
    VkQueue getPresentQueue() const;

private:
    void pickPDevice();
    bool isPDeviceSuitable(const VkPhysicalDevice& pDevice);
    void findQueueFamilies(const VkPhysicalDevice& pDevice);
    bool checkDeviceExts(VkPhysicalDevice pDevice);
    void createLDevice();

    VgeInstance& m_vgeInstance;
    VgeSurface& m_vgeSurface;
    VgeValidationLayers& m_vgeValidationLayers;

    VkInstance m_instance;
    VkSurfaceKHR m_surface;
    bool m_enableVLayers;
    std::vector<const char*> m_vLayers;

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

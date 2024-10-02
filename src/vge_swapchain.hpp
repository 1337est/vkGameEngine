#pragma once
#include "vge_queue_families.hpp"
#include "vge_surface.hpp"
#include "vge_window.hpp"
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace vge
{
class VgeSwapChain
{
public:
    VgeSwapChain(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VgeWindow& window,
        VgeSurface& surface,
        VgeQueueFamilies& queueFamilies);
    ~VgeSwapChain();

    VgeSwapChain(const VgeSwapChain&) = delete;
    VgeSwapChain& operator=(const VgeSwapChain&) = delete;

    VkSwapchainKHR getSwapChain() const;
    const std::vector<VkImage>& getSwapChainImages() const;

private:
    void createSwapChain();
    void querySwapChainSupport();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    VkSurfaceCapabilitiesKHR m_surfaceCapabilities{};
    std::vector<VkSurfaceFormatKHR> m_surfaceFormats{};
    std::vector<VkPresentModeKHR> m_presentModes{};

    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;
    VgeWindow& m_window;
    VgeSurface& m_surface;
    VgeQueueFamilies& m_queueFamilies;

    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapChainImages;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    VkExtent2D m_windowExtent;
};
} // namespace vge

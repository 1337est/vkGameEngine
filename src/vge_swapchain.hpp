#pragma once
#include "vge_device.hpp"
#include "vge_surface.hpp"
#include "vge_window.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeSwapchain {
public:
    VgeSwapchain(VgeDevice& vgeDevice, VgeSurface& vgeSurface, VgeWindow& vgeWindow);
    VgeSwapchain(
        VgeDevice& vgeDevice,
        VgeSurface& vgeSurface,
        VgeWindow& vgeWindow,
        std::shared_ptr<VgeSwapchain> oldSwapchain);
    ~VgeSwapchain();

    VgeSwapchain(const VgeSwapchain&) = delete;
    VgeSwapchain& operator=(const VgeSwapchain&) = delete;

    VkSwapchainKHR getSwapchain() const;
    const std::vector<VkImage>& getSwapchainImages() const;
    VkFormat getSwapchainImageFormat() const;
    VkExtent2D getSwapchainExtent() const;

private:
    void createSwapchain();
    void querySwapchainSupport();

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& surfaceCaps);

    VgeDevice& m_vgeDevice;
    VgeSurface& m_vgeSurface;
    VgeWindow& m_vgeWindow;

    VkPhysicalDevice m_pDevice;
    VkSurfaceKHR m_surface;
    uint32_t m_graphicsFamily;
    uint32_t m_presentFamily;
    VkDevice m_lDevice;
    GLFWwindow* m_window;

    std::vector<VkSurfaceFormatKHR> m_surfaceFormats{};
    std::vector<VkPresentModeKHR> m_presentModes{};
    VkSurfaceCapabilitiesKHR m_surfaceCaps{};

    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::shared_ptr<VgeSwapchain> m_oldSwapchain;

    std::vector<VkImage> m_swapchainImages;
    VkFormat m_swapchainImageFormat;
    VkExtent2D m_swapchainExtent;
    VkExtent2D m_windowExtent;
};
} // namespace vge

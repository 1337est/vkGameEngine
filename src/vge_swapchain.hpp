#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeSwapchain {
public:
    VgeSwapchain(
        VkPhysicalDevice pDevice,
        VkSurfaceKHR surface,
        uint32_t graphicsFamily,
        uint32_t presentFamily,
        VkDevice lDevice,
        GLFWwindow* window);
    ~VgeSwapchain();

    VgeSwapchain(const VgeSwapchain&) = delete;
    VgeSwapchain& operator=(const VgeSwapchain&) = delete;

    VkSwapchainKHR getSwapchain() const;
    const std::vector<VkImage>& getSwapchainImages() const;
    VkFormat getSwapchainImageFormat() const;

private:
    void createSwapchain();
    void querySwapchainSupport();

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& surfaceCaps);

    std::vector<VkSurfaceFormatKHR> m_surfaceFormats{};
    std::vector<VkPresentModeKHR> m_presentModes{};
    VkSurfaceCapabilitiesKHR m_surfaceCaps{};

    VkPhysicalDevice m_pDevice;
    VkSurfaceKHR m_surface;

    uint32_t m_graphicsFamily;
    uint32_t m_presentFamily;
    VkDevice m_lDevice;

    GLFWwindow* m_window;

    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapchainImages;
    VkFormat m_swapchainImageFormat;
    VkExtent2D m_swapchainExtent;
    VkExtent2D m_windowExtent;
};
} // namespace vge

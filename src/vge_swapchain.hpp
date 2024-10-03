#pragma once
#include <GLFW/glfw3.h>
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
        VkDevice logicalDevice,
        GLFWwindow* window,
        VkSurfaceKHR surface,
        uint32_t graphicsFamily,
        uint32_t presentFamily);
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
    VkDevice m_logicalDevice;
    GLFWwindow* m_window;
    VkSurfaceKHR m_surface;
    uint32_t m_graphicsFamily;
    uint32_t m_presentFamily;

    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapChainImages;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    VkExtent2D m_windowExtent;
};
} // namespace vge

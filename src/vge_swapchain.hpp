#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeSwapChain {
public:
    VgeSwapChain(
        VkPhysicalDevice pDevice,
        VkSurfaceKHR surface,
        uint32_t gFamily,
        uint32_t pFamily,
        VkDevice lDevice,
        GLFWwindow* window);
    ~VgeSwapChain();

    VgeSwapChain(const VgeSwapChain&) = delete;
    VgeSwapChain& operator=(const VgeSwapChain&) = delete;

    VkSwapchainKHR getSwapChain() const;
    const std::vector<VkImage>& getSwapChainImages() const;

private:
    void createSwapChain();
    void querySwapChainSupport();

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& surfaceCaps);

    std::vector<VkSurfaceFormatKHR> m_surfaceFormats{};
    std::vector<VkPresentModeKHR> m_presentModes{};
    VkSurfaceCapabilitiesKHR m_surfaceCaps{};

    VkPhysicalDevice m_pDevice;
    VkSurfaceKHR m_surface;

    uint32_t m_gFamily;
    uint32_t m_pFamily;
    VkDevice m_lDevice;

    GLFWwindow* m_window;

    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapChainImages;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    VkExtent2D m_windowExtent;
};
} // namespace vge

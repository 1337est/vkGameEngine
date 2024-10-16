#pragma once

#include "vge_device.hpp"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace vge {

class VgeSwapChain {
public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    VgeSwapChain(VgeDevice& deviceRef, VkExtent2D windowExtent);
    VgeSwapChain(
        VgeDevice& deviceRef,
        VkExtent2D windowExtent,
        std::shared_ptr<VgeSwapChain> previous);
    ~VgeSwapChain();

    VgeSwapChain(const VgeSwapChain&) = delete;
    VgeSwapChain& operator=(const VgeSwapChain&) = delete;

    VkFramebuffer getFrameBuffer(size_t index);
    VkRenderPass getRenderPass();
    VkImageView getImageView(size_t index);
    size_t imageCount();
    VkFormat getSwapChainImageFormat();
    VkExtent2D getSwapChainExtent();
    uint32_t width();
    uint32_t height();
    float extentAspectRatio();
    VkFormat findDepthFormat();
    VkResult acquireNextImage(uint32_t* imageIndex);

    VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

    bool compareSwapFormats(const VgeSwapChain& swapChain) const;

private:
    void initSwapChain();
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

    // Helper functions
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    VkFormat m_swapChainImageFormat;
    VkFormat m_swapChainDepthFormat;
    VkExtent2D m_swapChainExtent;

    std::vector<VkFramebuffer> m_swapChainFramebuffers;
    VkRenderPass m_renderPass;

    std::vector<VkImage> m_depthImages;
    std::vector<VkDeviceMemory> m_depthImageMemorys;
    std::vector<VkImageView> m_depthImageViews;
    std::vector<VkImage> m_swapChainImages;
    std::vector<VkImageView> m_swapChainImageViews;

    VgeDevice& m_device;
    VkExtent2D m_windowExtent;

    VkSwapchainKHR m_swapChain;
    std::shared_ptr<VgeSwapChain> m_oldSwapChain;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    std::vector<VkFence> m_imagesInFlight;
    size_t m_currentFrame = 0;
};

} // namespace vge

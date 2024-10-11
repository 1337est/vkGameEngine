#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeFramebuffer {
public:
    VgeFramebuffer(
        VkDevice lDevice,
        std::vector<VkImageView> swapchainImageViews,
        VkExtent2D swapchainExtent,
        VkRenderPass renderPass);
    ~VgeFramebuffer();
    VgeFramebuffer(const VgeFramebuffer&) = delete;
    VgeFramebuffer& operator=(const VgeFramebuffer&) = delete;

    std::vector<VkFramebuffer> getFramebuffers();

private:
    void createFramebuffers();

    // this block are for constructor initializers
    VkDevice m_lDevice;
    std::vector<VkImageView> m_swapchainImageViews;
    VkExtent2D m_swapchainExtent;
    VkRenderPass m_renderPass;

    std::vector<VkFramebuffer> m_framebuffers;
};
} // namespace vge

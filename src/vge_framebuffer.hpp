#pragma once

#include "vge_device.hpp"
#include "vge_image_views.hpp"
#include "vge_render_pass.hpp"
#include "vge_swapchain.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeFramebuffer {
public:
    VgeFramebuffer(
        VgeDevice& vgeDevice,
        VgeImageView& vgeImageView,
        VgeSwapchain& vgeSwapchain,
        VgeRenderPass& vgeRenderPass);
    ~VgeFramebuffer();
    VgeFramebuffer(const VgeFramebuffer&) = delete;
    VgeFramebuffer& operator=(const VgeFramebuffer&) = delete;

    std::vector<VkFramebuffer> getFramebuffers();

private:
    void createFramebuffers();

    VgeDevice& m_vgeDevice;
    VgeImageView& m_vgeImageView;
    VgeSwapchain& m_vgeSwapchain;
    VgeRenderPass& m_vgeRenderPass;

    // this block are for constructor initializers
    VkDevice m_lDevice;
    std::vector<VkImageView> m_imageViews;
    VkExtent2D m_swapchainExtent;
    VkRenderPass m_renderPass;

    std::vector<VkFramebuffer> m_framebuffers;
};
} // namespace vge

#include "vge_framebuffer.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vge {
VgeFramebuffer::VgeFramebuffer(
    VgeDevice& vgeDevice,
    VgeImageView& vgeImageView,
    VgeSwapchain& vgeSwapchain,
    VgeRenderPass& vgeRenderPass)
    : m_vgeDevice{ vgeDevice }
    , m_vgeImageView{ vgeImageView }
    , m_vgeSwapchain{ vgeSwapchain }
    , m_vgeRenderPass{ vgeRenderPass }
    , m_lDevice{ m_vgeDevice.getLDevice() }
    , m_imageViews{ m_vgeImageView.getImageViews() }
    , m_swapchainExtent{ m_vgeSwapchain.getSwapchainExtent() }
    , m_renderPass{ m_vgeRenderPass.getRenderPass() }
{
    createFramebuffers();
}

VgeFramebuffer::~VgeFramebuffer()
{
    for (auto framebuffer : m_framebuffers) {
        vkDestroyFramebuffer(m_lDevice, framebuffer, nullptr);
    }
}

void VgeFramebuffer::createFramebuffers()
{
    m_framebuffers.resize(m_imageViews.size());
    for (size_t i = 0; i < m_imageViews.size(); i++) {
        VkImageView attachments[] = { m_imageViews[i] };

        VkFramebufferCreateInfo framebufferInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = m_renderPass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = m_swapchainExtent.width,
            .height = m_swapchainExtent.height,
            .layers = 1,
        };

        if (vkCreateFramebuffer(m_lDevice, &framebufferInfo, nullptr, &m_framebuffers[i]) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

std::vector<VkFramebuffer> VgeFramebuffer::getFramebuffers()
{
    return m_framebuffers;
}
} // namespace vge

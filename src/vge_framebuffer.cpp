#include "vge_framebuffer.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vge {
VgeFramebuffer::VgeFramebuffer(
    VkDevice lDevice,
    std::vector<VkImageView> swapchainImageViews,
    VkExtent2D swapchainExtent,
    VkRenderPass renderPass)
    : m_lDevice{ lDevice }
    , m_swapchainImageViews{ swapchainImageViews }
    , m_swapchainExtent{ swapchainExtent }
    , m_renderPass{ renderPass }
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
    m_framebuffers.resize(m_swapchainImageViews.size());
    for (size_t i = 0; i < m_swapchainImageViews.size(); i++) {
        VkImageView attachments[] = { m_swapchainImageViews[i] };

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

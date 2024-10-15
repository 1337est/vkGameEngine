#include "vge_render_pass.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vge {
VgeRenderPass::VgeRenderPass(VkDevice lDevice, VkFormat swapchainImageFormat)
    : m_lDevice{ lDevice }
    , m_swapchainImageFormat{ swapchainImageFormat }
{
    createRenderPass();
}

VgeRenderPass::~VgeRenderPass()
{
    vkDestroyRenderPass(m_lDevice, m_renderPass, nullptr);
}

void VgeRenderPass::createRenderPass()
{
    VkAttachmentDescription colorAttachment{
        .flags = 0,
        .format = m_swapchainImageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkAttachmentReference colorAttachmentRef{
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass{
        .flags = 0, // default
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,    // default
        .pInputAttachments = nullptr, // default
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef,
        .pResolveAttachments = nullptr,     // default
        .pDepthStencilAttachment = nullptr, // default
        .preserveAttachmentCount = 0,       // default
        .pPreserveAttachments = nullptr,    // default
    };
    VkSubpassDependency subpassDependency{
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
    };

    VkRenderPassCreateInfo renderPassCI{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .attachmentCount = 1,
        .pAttachments = &colorAttachment,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &subpassDependency,
    };

    if (vkCreateRenderPass(m_lDevice, &renderPassCI, nullptr, &m_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass!");
    }
}

VkRenderPass VgeRenderPass::getRenderPass() const
{
    return m_renderPass;
}
} // namespace vge

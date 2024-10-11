#include "vge_command_pool.hpp"
#include <stdexcept>

namespace vge {
VgeCommandPool::VgeCommandPool(
    VkDevice lDevice,
    uint32_t graphicsFamily,
    VkRenderPass renderPass,
    std::vector<VkFramebuffer> framebuffers,
    VkExtent2D swapchainExtent,
    VkPipeline graphicsPipeline)
    : m_lDevice{ lDevice }
    , m_graphicsFamily{ graphicsFamily }
    , m_renderPass{ renderPass }
    , m_framebuffers{ framebuffers }
    , m_swapchainExtent{ swapchainExtent }
    , m_graphicsPipeline{ graphicsPipeline }
{
    createCommandPool();
    createCommandBuffer();
}

VgeCommandPool::~VgeCommandPool()
{
    // command buffers destroyed when pool is destroyed
    vkDestroyCommandPool(m_lDevice, m_commandPool, nullptr);
}

void VgeCommandPool::createCommandPool()
{
    VkCommandPoolCreateInfo commandPoolCI = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = m_graphicsFamily,
    };

    if (vkCreateCommandPool(m_lDevice, &commandPoolCI, nullptr, &m_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }
}

void VgeCommandPool::createCommandBuffer()
{
    VkCommandBufferAllocateInfo commandBufferAllocInfo{};
    commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocInfo.commandPool = m_commandPool;
    commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(m_lDevice, &commandBufferAllocInfo, &m_commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void VgeCommandPool::recordCommandBuffer(uint32_t imageIndex)
{
    VkCommandBufferBeginInfo commandBufferBeginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,                  // Optional
        .pInheritanceInfo = nullptr, // Optional
    };

    if (vkBeginCommandBuffer(m_commandBuffer, &commandBufferBeginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
    VkRenderPassBeginInfo renderPassBeginInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = m_renderPass,
        .framebuffer = m_framebuffers[imageIndex],
        .renderArea = {
            .offset = { 0, 0 },
            .extent =  m_swapchainExtent,
        },
        .clearValueCount = 1,
        .pClearValues = &clearColor,
    };

    vkCmdBeginRenderPass(m_commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

    VkViewport viewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(m_swapchainExtent.width),
        .height = static_cast<float>(m_swapchainExtent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{
        .offset = { 0, 0 },
        .extent = m_swapchainExtent,
    };
    vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);

    vkCmdDraw(m_commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(m_commandBuffer);

    if (vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

VkCommandBuffer VgeCommandPool::getCommandBuffer() const
{
    return m_commandBuffer;
}
} // namespace vge

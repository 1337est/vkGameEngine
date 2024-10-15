#include "vge_command_buffer.hpp"
#include <stdexcept>

namespace vge {
VgeCommandBuffer::VgeCommandBuffer(VkDevice lDevice, VkCommandPool commandPool)
    : m_lDevice{ lDevice }
    , m_commandPool{ commandPool }
{
    createCommandBuffer();
}

void VgeCommandBuffer::createCommandBuffer()
{
    VkCommandBufferAllocateInfo commandBufferAllocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = m_commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    if (vkAllocateCommandBuffers(m_lDevice, &commandBufferAllocInfo, &m_commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

VkCommandBuffer VgeCommandBuffer::getCommandBuffer() const
{
    return m_commandBuffer;
}
} // namespace vge

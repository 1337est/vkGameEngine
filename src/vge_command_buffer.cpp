#include "vge_command_buffer.hpp"
#include "max_frames_in_flight.hpp"
#include <stdexcept>

namespace vge {
VgeCommandBuffer::VgeCommandBuffer(VgeDevice& vgeDevice, VgeCommandPool& vgeCommandPool)
    : m_vgeDevice{ vgeDevice }
    , m_vgeCommandPool{ vgeCommandPool }
    , m_lDevice{ m_vgeDevice.getLDevice() }
    , m_commandPool{ m_vgeCommandPool.getCommandPool() }
{
    createCommandBuffer();
}

void VgeCommandBuffer::createCommandBuffer()
{
    m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo commandBufferAllocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = m_commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t)m_commandBuffers.size(),
    };

    if (vkAllocateCommandBuffers(m_lDevice, &commandBufferAllocInfo, m_commandBuffers.data()) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

std::vector<VkCommandBuffer> VgeCommandBuffer::getCommandBuffers() const
{
    return m_commandBuffers;
}
} // namespace vge

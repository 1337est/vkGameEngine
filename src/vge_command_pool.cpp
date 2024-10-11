#include "vge_command_pool.hpp"
#include <stdexcept>

namespace vge {
VgeCommandPool::VgeCommandPool(VkDevice lDevice, uint32_t graphicsFamily)
    : m_lDevice{ lDevice }
    , m_graphicsFamily{ graphicsFamily }
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
} // namespace vge

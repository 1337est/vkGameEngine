#include "vge_command_pool.hpp"
#include <stdexcept>

namespace vge {
VgeCommandPool::VgeCommandPool(VkDevice lDevice, uint32_t graphicsFamily)
    : m_lDevice{ lDevice }
    , m_graphicsFamily{ graphicsFamily }
{
    createCommandPool();
}

VgeCommandPool::~VgeCommandPool()
{
    vkDestroyCommandPool(m_lDevice, m_commandPool, nullptr);
}

void VgeCommandPool::createCommandPool()
{
    VkCommandPoolCreateInfo commandPoolCI = {};
    commandPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCI.queueFamilyIndex = m_graphicsFamily;
    commandPoolCI.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(m_lDevice, &commandPoolCI, nullptr, &m_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }
}
} // namespace vge

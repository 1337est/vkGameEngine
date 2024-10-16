#include "vge_command_pool.hpp"
#include <stdexcept>

namespace vge {
VgeCommandPool::VgeCommandPool(VgeDevice& vgeDevice)
    : m_vgeDevice{ vgeDevice }
    , m_lDevice{ m_vgeDevice.getLDevice() }
    , m_graphicsFamily{ m_vgeDevice.getGraphicsFamily() }
{
    createCommandPool();
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
        .flags =
            VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = m_graphicsFamily,
    };

    if (vkCreateCommandPool(m_lDevice, &commandPoolCI, nullptr, &m_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }
}

VkCommandPool VgeCommandPool::getCommandPool() const
{
    return m_commandPool;
}
} // namespace vge

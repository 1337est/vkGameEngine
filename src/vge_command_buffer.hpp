#pragma once

#include "vge_command_pool.hpp"
#include "vge_device.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeCommandBuffer {
public:
    VgeCommandBuffer(VgeDevice& vgeDevice, VgeCommandPool& vgeCommandPool);
    VgeCommandBuffer(const VgeCommandBuffer&) = delete;
    VgeCommandBuffer& operator=(const VgeCommandBuffer&) = delete;

    std::vector<VkCommandBuffer> getCommandBuffers() const;

private:
    void createCommandBuffer();

    VgeDevice& m_vgeDevice;
    VgeCommandPool& m_vgeCommandPool;

    VkDevice m_lDevice;
    VkCommandPool m_commandPool;

    std::vector<VkCommandBuffer> m_commandBuffers;
};
} // namespace vge

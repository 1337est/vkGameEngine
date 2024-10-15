#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeCommandBuffer {
public:
    VgeCommandBuffer(VkDevice lDevice, VkCommandPool commandPool);
    VgeCommandBuffer(const VgeCommandBuffer&) = delete;
    VgeCommandBuffer& operator=(const VgeCommandBuffer&) = delete;

    std::vector<VkCommandBuffer> getCommandBuffers() const;

private:
    void createCommandBuffer();

    VkDevice m_lDevice;
    VkCommandPool m_commandPool;

    std::vector<VkCommandBuffer> m_commandBuffers;
};
} // namespace vge

#pragma once

#include <vulkan/vulkan_core.h>

namespace vge {
class VgeCommandPool {
public:
    VgeCommandPool(VkDevice lDevice, uint32_t graphicsFamily);
    ~VgeCommandPool();
    VgeCommandPool(const VgeCommandPool&) = delete;
    VgeCommandPool& operator=(const VgeCommandPool&) = delete;

    VkCommandPool getCommandPool() const;
    VkCommandBuffer getCommandBuffer() const;

private:
    void createCommandPool();
    void createCommandBuffer();

    VkDevice m_lDevice;
    uint32_t m_graphicsFamily;

    VkCommandPool m_commandPool;
    VkCommandBuffer m_commandBuffer;
};
} // namespace vge

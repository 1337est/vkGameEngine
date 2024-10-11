#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeCommandPool {
public:
    VgeCommandPool(
        VkDevice lDevice,
        uint32_t graphicsFamily,
        VkRenderPass renderPass,
        std::vector<VkFramebuffer> framebuffers,
        VkExtent2D swapchainExtent,
        VkPipeline graphicsPipeline);
    ~VgeCommandPool();
    VgeCommandPool(const VgeCommandPool&) = delete;
    VgeCommandPool& operator=(const VgeCommandPool&) = delete;

    VkCommandPool getCommandPool() const;
    VkCommandBuffer getCommandBuffer() const;
    void recordCommandBuffer(uint32_t imageIndex);

private:
    void createCommandPool();
    void createCommandBuffer();

    VkDevice m_lDevice;
    uint32_t m_graphicsFamily;
    VkRenderPass m_renderPass;
    std::vector<VkFramebuffer> m_framebuffers;
    VkExtent2D m_swapchainExtent;
    VkPipeline m_graphicsPipeline;

    VkCommandPool m_commandPool;
    VkCommandBuffer m_commandBuffer;
};
} // namespace vge

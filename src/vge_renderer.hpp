#pragma once

#include <vulkan/vulkan_core.h>

namespace vge {
class VgeRenderer {
public:
    VgeRenderer();

private:
    void drawFrame();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void recreateSwapchain();
    void cleanupSwapchain();
};
} // namespace vge

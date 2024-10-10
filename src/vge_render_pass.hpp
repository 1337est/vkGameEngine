#pragma once

#include <vulkan/vulkan_core.h>

namespace vge {
class VgeRenderPass {
public:
    VgeRenderPass(VkDevice lDevice, VkFormat swapchainImageFormat);
    ~VgeRenderPass();
    VgeRenderPass(const VgeRenderPass&) = delete;
    VgeRenderPass& operator=(const VgeRenderPass&) = delete;

    VkRenderPass getRenderPass() const;

private:
    void createRenderPass();

    VkDevice m_lDevice;
    VkFormat m_swapchainImageFormat;

    VkRenderPass m_renderPass;
};
} // namespace vge

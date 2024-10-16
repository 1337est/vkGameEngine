#pragma once

#include "vge_device.hpp"
#include "vge_swapchain.hpp"
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeRenderPass {
public:
    VgeRenderPass(VgeDevice& vgeDevice, VgeSwapchain& vgeSwapchain);
    ~VgeRenderPass();
    VgeRenderPass(const VgeRenderPass&) = delete;
    VgeRenderPass& operator=(const VgeRenderPass&) = delete;

    VkRenderPass getRenderPass() const;

private:
    void createRenderPass();

    VgeDevice& m_vgeDevice;
    VgeSwapchain& m_vgeSwapchain;

    VkDevice m_lDevice;
    VkFormat m_swapchainImageFormat;

    VkRenderPass m_renderPass;
};
} // namespace vge

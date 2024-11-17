#pragma once

#include "vge_command_buffer.hpp"
#include "vge_command_pool.hpp"
#include "vge_device.hpp"
#include "vge_framebuffer.hpp"
#include "vge_image_views.hpp"
#include "vge_pipeline.hpp"
#include "vge_render_pass.hpp"
#include "vge_surface.hpp"
#include "vge_swapchain.hpp"
#include "vge_sync_objects.hpp"
#include "vge_window.hpp"
#include <memory>
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeRenderer {
public:
    VgeRenderer(
        VgeDevice& vgeDevice,
        VgeCommandPool& vgeCommandPool,
        VgePipeline& vgePipeline,
        VgeWindow& vgeWindow,
        VgeSurface& vgeSurface);

private:
    void drawFrame();

    void recreateSwapchain();
    void recreateImageViews();
    void recreateRenderPass();
    void recreateFramebuffers();
    void recreateSyncObjects();

    void cleanupSwapchain();
    void cleanupImageViews();
    void cleanupRenderPass();
    void cleanupFramebuffers();
    void cleanupSyncObjects();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    VgeDevice& m_vgeDevice;
    VgeCommandPool& m_vgeCommandPool;
    VgePipeline& m_vgePipeline;
    VgeWindow& m_vgeWindow;
    VgeSurface& m_vgeSurface;

    std::unique_ptr<VgeSwapchain> m_uniqueVgeSwapchain;
    std::unique_ptr<VgeImageView> m_uniqueVgeImageView;
    std::unique_ptr<VgeRenderPass> m_uniqueVgeRenderPass;
    std::unique_ptr<VgeFramebuffer> m_uniqueVgeFramebuffer;
    std::unique_ptr<VgeSyncObjects> m_uniqueVgeSyncObjects;
    std::unique_ptr<VgeCommandBuffer> m_uniqueVgeCommandBuffers;
};
} // namespace vge

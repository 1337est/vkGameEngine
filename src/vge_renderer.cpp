#include "vge_renderer.hpp"

namespace vge {
VgeRenderer::VgeRenderer(
    VgeDevice& vgeDevice,
    VgeCommandPool& vgeCommandPool,
    VgePipeline& vgePipeline,
    VgeWindow& vgeWindow,
    VgeSurface& vgeSurface)
    : m_vgeDevice{ vgeDevice }
    , m_vgeCommandPool{ vgeCommandPool }
    , m_vgePipeline{ vgePipeline }
    , m_vgeWindow{ vgeWindow }
    , m_vgeSurface{ vgeSurface }
    , m_uniqueVgeSwapchain{}
    , m_uniqueVgeImageView{}
    , m_uniqueVgeRenderPass{}
    , m_uniqueVgeFramebuffer{}
    , m_uniqueVgeSyncObjects{}
    , m_uniqueVgeCommandBuffers{}
{
    recreateSwapchain();
    recreateImageViews();
    recreateRenderPass();
    recreateFramebuffers();
    recreateSyncObjects();
}
} // namespace vge

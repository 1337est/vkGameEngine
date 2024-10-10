#pragma once
#include "vge_device.hpp"
#include "vge_framebuffer.hpp"
#include "vge_image_views.hpp"
#include "vge_instance.hpp"
#include "vge_pipeline.hpp"
#include "vge_render_pass.hpp"
#include "vge_surface.hpp"
#include "vge_swapchain.hpp"
#include "vge_window.hpp"

namespace vge {

class VgeApp {
public:
    // windows width/height
    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;

    VgeApp();

    void run();

private:
    VgeWindow m_vgeWindow;
    VgeInstance m_vgeInstance;
    VgeSurface m_vgeSurface;
    VgeDevice m_vgeDevice;
    VgeSwapchain m_vgeSwapchain;
    VgeImageView m_vgeImageView;
    VgeRenderPass m_vgeRenderPass;
    VgePipeline m_vgePipeline;
    VgeFramebuffer m_vgeFramebuffer;
};

} // namespace vge

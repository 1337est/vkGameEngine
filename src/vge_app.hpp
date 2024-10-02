#pragma once
#include "vge_device.hpp"
#include "vge_instance.hpp"
#include "vge_pipeline.hpp"
#include "vge_queue_families.hpp"
#include "vge_surface.hpp"
#include "vge_swapchain.hpp"
#include "vge_validation_layers.hpp"
#include "vge_window.hpp"

namespace vge
{

class VgeApp
{
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
    VgeValidationLayers m_vgeValidationLayers;
    VgeDevice m_vgeDevice;
    VgeQueueFamilies m_vgeQueueFamilies;
    VgeSwapChain m_vgeSwapChain;
    VgePipeline m_vgePipeline;
};

} // namespace vge

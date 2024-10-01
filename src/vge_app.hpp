#pragma once
#include "vge_device.hpp"
#include "vge_instance.hpp"
#include "vge_pipeline.hpp"
#include "vge_surface.hpp"
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

    void run();

private:
    // Creates a VgeWindow with width, height, and title
    VgeWindow m_vgeWindow{ WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Vulkan!" };

    // Creates a Vulkan instance
    VgeInstance m_vgeInstance;

    // Creates a Vulkan surface
    VgeSurface m_vgeSurface{ m_vgeInstance, m_vgeWindow };

    // Validation layers for the Vulkan app
    VgeValidationLayers m_vgeValidationLayers;

    // Creates a Vulkan device
    VgeDevice m_vgeDevice{
        m_vgeInstance.getInstance(),
        m_vgeSurface.getSurface(),
        m_vgeValidationLayers,
    };

    // Creates a VgePipeline with the vertex and fragment shaders
    VgePipeline m_vgePipeline{ "build/shaders/shader.vert.spv",
                               "build/shaders/shader.frag.spv" };
};

} // namespace vge

#pragma once

// headers
#include "vge_instance.hpp"
#include "vge_pipeline.hpp"
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
    // Validation layers (only active in debug builds)
    VgeValidationLayers m_vgeValidationLayers;

    // Creates a Vulkan instance
    VgeInstance m_vgeInstance;

    // Creates a VgePipeline with the vertex and fragment shaders
    VgePipeline m_vgePipeline{ "build/shaders/shader.vert.spv",
                               "build/shaders/shader.frag.spv" };

    // Creates a VgeWindow with width, height, and title
    VgeWindow m_vgeWindow{ WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Vulkan!" };
};

} // namespace vge

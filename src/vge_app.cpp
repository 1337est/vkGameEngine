#include "vge_app.hpp"

namespace vge {
VgeApp::VgeApp()
    : m_vgeWindow{ WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Vulkan!" }
    , m_vgeInstance{}
    , m_vgeSurface{ m_vgeInstance.getInstance(), m_vgeWindow.getWindow() }
    , m_vgeDevice{ m_vgeInstance.getInstance(),
                   m_vgeSurface.getSurface(),
                   m_vgeInstance.areVLayersEnabled(),
                   m_vgeInstance.getVLayers() }
    , m_vgeSwapChain{ m_vgeDevice.getPDevice(), m_vgeSurface.getSurface(), m_vgeDevice.getGFamily(),
                      m_vgeDevice.getPFamily(), m_vgeDevice.getLDevice(),  m_vgeWindow.getWindow() }
    , m_vgePipeline{ "build/shaders/shader.vert.spv", "build/shaders/shader.frag.spv" }
{}

void VgeApp::run()
{
    // run until window closes
    while (!m_vgeWindow.shouldClose()) {
        glfwPollEvents(); // continuously processes and returns received events
    }
}

} // namespace vge

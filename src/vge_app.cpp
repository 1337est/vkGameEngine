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
    , m_vgeSwapchain{ m_vgeDevice.getPDevice(),       m_vgeSurface.getSurface(), m_vgeDevice.getGraphicsFamily(),
                      m_vgeDevice.getPresentFamily(), m_vgeDevice.getLDevice(),  m_vgeWindow.getWindow() }
    , m_vgeImageView{ m_vgeDevice.getLDevice(),
                      m_vgeSwapchain.getSwapchainImages(),
                      m_vgeSwapchain.getSwapchainImageFormat() }
    , m_vgeRenderPass(m_vgeDevice.getLDevice(), m_vgeSwapchain.getSwapchainImageFormat())
    , m_vgePipeline{ m_vgeDevice.getLDevice(),
                     "build/shaders/shader.vert.spv",
                     "build/shaders/shader.frag.spv",
                     m_vgeRenderPass.getRenderPass() }
    , m_vgeFramebuffer{ m_vgeDevice.getLDevice(),
                        m_vgeImageView.getSwapchainImageViews(),
                        m_vgeSwapchain.getSwapchainExtent(),
                        m_vgeRenderPass.getRenderPass() }
    , m_vgeCommandPool{ m_vgeDevice.getLDevice(), m_vgeDevice.getGraphicsFamily() }

{}

void VgeApp::run()
{
    // run until window closes
    while (!m_vgeWindow.shouldClose()) {
        glfwPollEvents(); // continuously processes and returns received events
    }
}

} // namespace vge

#include "vge_app.hpp"
#include <iostream>

namespace vge
{
VgeApp::VgeApp()
    : m_vgeWindow{ WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Vulkan!" }
    , m_vgeSurface{ m_vgeInstance.getInstance(), m_vgeWindow.getGLFWwindow() }
    , m_vgeDevice{ m_vgeInstance.getInstance(),
                   m_vgeSurface.getSurface(),
                   m_vgeValidationLayers.areValidationLayersEnabled(),
                   m_vgeValidationLayers.getValidationLayers() }
    , m_vgeQueueFamilies{ m_vgeDevice.getPhysicalDevice(),
                          m_vgeSurface.getSurface() }
    , m_vgeSwapChain{ m_vgeDevice.getPhysicalDevice(),
                      m_vgeDevice.getLogicalDevice(),
                      m_vgeWindow.getGLFWwindow(),
                      m_vgeSurface.getSurface(),
                      m_vgeQueueFamilies.getGraphicsFamily(),
                      m_vgeQueueFamilies.getPresentFamily() }
    , m_vgePipeline{ "build/shaders/shader.vert.spv",
                     "build/shaders/shader.frag.spv" }
{
}

void VgeApp::run()
{
    std::cout << "Application is running. Waiting for window events...";

    // run until window closes
    while (!m_vgeWindow.shouldClose())
    {
        glfwPollEvents(); // continuously processes and returns received events
    }

    std::cout << "Application loop exited. Window closed.\n\n";
}

} // namespace vge

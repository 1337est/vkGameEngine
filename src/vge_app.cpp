#include "vge_app.hpp"
#include <iostream>

namespace vge
{
VgeApp::VgeApp()
    : m_vgeWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Vulkan!")
    , m_vgeSurface(m_vgeInstance, m_vgeWindow)
    , m_vgeDevice(
          m_vgeInstance.getInstance(),
          m_vgeSurface.getSurface(),
          m_vgeValidationLayers)
    , m_vgePipeline(
          "build/shaders/shader.vert.spv",
          "build/shaders/shader.frag.spv")
{
    std::cout << "VgeApp Constructor: " << "\nWindow dimensions: "
              << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << "\nInstance created: "
              << (m_vgeInstance.getInstance() != VK_NULL_HANDLE)
              << "\nSurface created: "
              << (m_vgeSurface.getSurface() != VK_NULL_HANDLE)
              << "\nDevice initialized." << std::endl;
}

void VgeApp::run()
{
    std::cout << "Application is running. Waiting for window events..."
              << std::endl;

    // run until window closes
    while (!m_vgeWindow.shouldClose())
    {
        glfwPollEvents(); // continuously processes and returns received events
    }

    std::cout << "Application loop exited. Window closed." << std::endl;
}

} // namespace vge

// headers
#include "vge_window.hpp"

// libraries
#include <GLFW/glfw3.h>

// std
#include <stdexcept>

namespace vge
{

VgeWindow::VgeWindow(int width, int height, std::string name)
    : m_window{ nullptr }
    , m_width{ width }
    , m_height{ height }
    , m_name(name)
{
    initWindow();
}

VgeWindow::~VgeWindow()
{
    glfwDestroyWindow(m_window); // Destroys our window context on exit
    glfwTerminate(); // free all remaining resources and uninitialize glfw
}

void VgeWindow::initWindow()
{
    glfwInit();                                   // initialize glfw library
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Don't create OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);    // No resizeable windows

    // create the window
    m_window =
        glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, frameBufferResizeCallback);
}

void VgeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
    // Create the surface in the window, enabling the display of rendered images
    if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface");
    }
}

void VgeWindow::frameBufferResizeCallback(
    GLFWwindow* window,
    int width,
    int height)
{
    auto vgeWindow =
        reinterpret_cast<VgeWindow*>(glfwGetWindowUserPointer(window));
    vgeWindow->m_frameBufferResized = true;
    vgeWindow->m_width = width;
    vgeWindow->m_height = height;
}

} // namespace vge

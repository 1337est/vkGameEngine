#include "vge_window.hpp"
#include <iostream>

namespace vge
{

VgeWindow::VgeWindow(int width, int height, const std::string& name)
    : m_width{ width }
    , m_height{ height }
    , m_name{ name }
{
    std::cout << "VgeWindow Constructor: Initializing window..." << std::endl;
    initWindow();
    std::cout << "VgeWindow Constructor: "
              << "\nWindow created with dimensions: " << m_width << "x"
              << m_height << "\nWindow name: " << m_name << std::endl;
}

VgeWindow::~VgeWindow()
{
    std::cout << "VgeWindow Destructor: Destroying window named: " << m_name
              << " with dimensions: " << m_width << "x" << m_height
              << std::endl;
    glfwDestroyWindow(m_window);
    glfwTerminate();
    std::cout << "VgeWindow Destructor: Window destruction complete."
              << std::endl;
}

void VgeWindow::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window =
        glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr);
}

bool VgeWindow::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

GLFWwindow* VgeWindow::getGLFWwindow() const
{
    return m_window;
}
} // namespace vge

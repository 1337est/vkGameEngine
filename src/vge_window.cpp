#include "vge_window.hpp"
#include <iostream>

namespace vge {

VgeWindow::VgeWindow(int width, int height, const std::string& name) :
    m_width{ width },
    m_height{ height },
    m_name{ name }
{
    std::cout << "START: VgeWindow Constructor\n";
    initWindow();
    std::cout << "Window initialized.\n";
    std::cout << "END: VgeWindow Constructor\n\n";
}

VgeWindow::~VgeWindow()
{
    std::cout << "START: VgeWindow Destructor\n";
    glfwDestroyWindow(m_window);
    glfwTerminate();
    std::cout << "Window destroyed.\n";
    std::cout << "END: VgeWindow Destructor\n\n";
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

#include "vge_window.hpp"

namespace vge
{

VgeWindow::VgeWindow(int width, int height, std::string name)
    : m_width{ width }
    , m_height{ height }
    , m_name{ name }
{
    initWindow();
}

VgeWindow::~VgeWindow()
{

    glfwDestroyWindow(m_window);
    glfwTerminate();
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

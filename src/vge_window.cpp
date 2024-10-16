#include "vge_window.hpp"

namespace vge {
// windows width/height
static constexpr int WIDTH = 800;
static constexpr int HEIGHT = 600;
const std::string& TITLE = "Hello Vulkan";

VgeWindow::VgeWindow()
    : m_width{ WIDTH }
    , m_height{ HEIGHT }
    , m_title{ TITLE }
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

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
}

bool VgeWindow::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

GLFWwindow* VgeWindow::getWindow() const
{
    return m_window;
}
} // namespace vge

#include "vge_window.hpp"
#include <GLFW/glfw3.h>

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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);

    // glfw window setter must be called before it's associated getter
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, frameBufferResizeCallback);
}

void VgeWindow::frameBufferResizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
    VgeWindow* vgeWindow = reinterpret_cast<VgeWindow*>(glfwGetWindowUserPointer(window));
    vgeWindow->m_frameBufferResized = true;
    vgeWindow->m_width = newWidth;
    vgeWindow->m_height = newHeight;
}

bool VgeWindow::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

GLFWwindow* VgeWindow::getWindow() const
{
    return m_window;
}

bool VgeWindow::wasWindowResized() const
{
    return m_frameBufferResized;
}

void VgeWindow::resetWindowResizedFlag()
{
    m_frameBufferResized = false;
}
} // namespace vge

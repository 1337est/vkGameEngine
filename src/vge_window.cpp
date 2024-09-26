// headers
#include "vge_window.hpp"

namespace vge
{

/* Constructs a `VgeWindow` object and initializes the GLFW window
 *
 * TODO: Description
 */
VgeWindow::VgeWindow(int width, int height, std::string name)
    : m_width{ width }
    , m_height{ height }
    , m_name(name)
{
    initWindow();
}

/* Cleans up and destroys the GLFW window
 *
 * TODO: Description
 */
VgeWindow::~VgeWindow()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

/* Initializes the GLFW window.
 *
 * TODO: Description
 */
void VgeWindow::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window =
        glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr);
}

/* Checks if the GLFW window should close.
 *
 * TODO: Description
 */
bool VgeWindow::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

} // namespace vge

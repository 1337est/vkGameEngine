// headers
#include "vge_window.hpp"

namespace vge
{

/* Constructs a `VgeWindow` object and initializes the GLFW window
 *
 * Sets the window's dimenstions and title, then initializes the window by
 * calling `initWindow()`.
 */
VgeWindow::VgeWindow(int width, int height, std::string name)
    : m_width{ width }
    , m_height{ height }
    , m_name{ name }
{
    initWindow();
}

/* Cleans up and destroys the GLFW window
 *
 * Ensures the GLFW window is properly destroyed, and terminates the GLFW
 * library to free any associated resources.
 */
VgeWindow::~VgeWindow()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

/* Initializes the GLFW window.
 *
 * This method creates a window with the specified width, height, and name. It
 * sets up the window to use Vulkan instead of OpenGL and makes it
 * non-resizable.
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
 * Returns true if the window is instructed to close, which typically occurs
 * when the user clicks the close button.
 */
bool VgeWindow::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

} // namespace vge

#include "vge_window.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>

namespace vge {

/* Constructs a `VgeWindow` object and initializes the GLFW window
 *
 * Initializes the `VgeWindow` instance and calls `initWindow()` to create and
 * set up the GLFW window.
 */
VgeWindow::VgeWindow(int width, int height, std::string name)
    : m_window{ nullptr }
    , m_width{ width }
    , m_height{ height }
    , m_name(name)
{
    initWindow();
}

/* Cleans up and destroys the GLFW window
 *
 * This destructor destroys our window context on exit, frees all remaining
 * resources, and uninitializes GLFW.
 */
VgeWindow::~VgeWindow()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

/* Initializes the GLFW window.
 *
 * This function initializes the GLFW library. It sets window hints to not
 * create an OpenGL context and to allow resizing the window. It then creates
 * the window and sets the window user pointer to `this` and registers a
 * framebuffer size callback to handle the window resizing events.
 */
void VgeWindow::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr);

    // glfw window setter must be called before it's associated getter
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, frameBufferResizeCallback);
}

/* Checks if the window was resized.
 *
 * Returns `true` if the framebuffer was resized, otherwise `false`.
 */
bool VgeWindow::wasWindowResized() const
{
    return m_frameBufferResized;
}

/* Handles framebuffer resize events.
 *
 * Updates the `VgeWindow` instance's dimensions and marks the framebuffer as
 * resized when the window is resized.
 */
void VgeWindow::frameBufferResizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
    VgeWindow* vgeWindow = reinterpret_cast<VgeWindow*>(glfwGetWindowUserPointer(window));
    vgeWindow->m_frameBufferResized = true;
    vgeWindow->m_width = newWidth;
    vgeWindow->m_height = newHeight;
}

/* Resets the window resized flag.
 *
 * Sets the `m_frameBufferResized` flag to `false`.
 */
void VgeWindow::resetWindowResizedFlag()
{
    m_frameBufferResized = false;
}

/* Gets the current extent (dimensions) of the GLFW window.
 *
 * Returns the current width and height of the window as a `VkExtent2D` struct.
 */
VkExtent2D VgeWindow::getExtent() const
{
    return { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height) };
}

/* Creates a Vulkan surface associated with the GLFW window.
 *
 * The Vulkan surface in the window allows Vulkan to render/display images to
 * the window. If the surface creation fails it throws an exception.
 */
void VgeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
    // Create the surface in the window, enabling the display of rendered images
    if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }
}

/* Checks if the GLFW window should close.
 *
 * Returns `true` if the GLFW window has requested to close, otherwise `false`.
 */
bool VgeWindow::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

/* Gets the GLFW window handle.
 *
 * Returns the `GLFWwindow*` handle for the current GLFW window.
 */
GLFWwindow* VgeWindow::getGLFWwindow() const
{
    return m_window;
}

} // namespace vge

```cpp
// headers
#include "vge_window.hpp"

// libs
#include <GLFW/glfw3.h>

// std
#include <stdexcept>

namespace vge
{

/** Constructs a `VgeWindow` object and initializes the GLFW window
 *
 * Initializes the `VgeWindow` instance and calls `initWindow()` to create and
 * set up the GLFW window.
 *
 * @param `width` The width of the window in pixels.
 * @param `height` The height of the window in pixels.
 * @param `name` The title of the window.
 */
VgeWindow::VgeWindow(int width, int height, std::string name)
    : m_window{ nullptr }
    , m_width{ width }
    , m_height{ height }
    , m_name(name)
{
    initWindow();
}

/** Cleans up and destroys the GLFW window
 *
 * This destructor destroys our window context on exit, frees all remaining
 * resources, and uninitializes GLFW.
 */
VgeWindow::~VgeWindow()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

/** Initializes the GLFW window.
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

    m_window =
        glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr);

    // glfw window setter must be called before it's associated getter
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, frameBufferResizeCallback);
}

/** Creates a Vulkan surface associated with the GLFW window.
 *
 * The Vulkan surface in the window allows Vulkan to render/display images to
 * the window. If the surface creation fails it throws an exception.
 *
 * @param `instance` The Vulkan instance used to create the surface.
 * @param `surface` A pointer to a `VkSurfaceKHR` handle that will be
 * initialized with the created surface
 */
void VgeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
    // Create the surface in the window, enabling the display of rendered images
    if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface");
    }
}

/** Callback function to handle framebuffer resize events.
 *
 * This function is called when the framebuffer is resized. It retrievs the
 * `VgeWindow*` instance from `glfwGetWindowUserPointer` associated with the
 * window set with `glfwSetWindowUserPointer`. It also updates the instance's
 * `m_frameBufferResized` flag to `true` and sets a new `m_width` and `m_height`
 * to reflect the new dimensions of the framebuffer.
 *
 * @param `window` The GLFW window that triggered the callback.
 * @param `newWidth` The new width of the framebuffer.
 * @param `newHeight` The new height of the framebuffer.
 */
void VgeWindow::frameBufferResizeCallback(
    GLFWwindow* window,
    int newWidth,
    int newHeight)
{
    VgeWindow* vgeWindow =
        reinterpret_cast<VgeWindow*>(glfwGetWindowUserPointer(window));
    vgeWindow->m_frameBufferResized = true;
    vgeWindow->m_width = newWidth;
    vgeWindow->m_height = newHeight;
}

} // namespace vge
```

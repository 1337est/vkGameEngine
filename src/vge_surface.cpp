#include "vge_surface.hpp"
#include <stdexcept>

namespace vge
{

VgeSurface::VgeSurface(VgeInstance& instance, VgeWindow& window)
    : m_instance{ instance }
    , m_window{ window }
{
    createSurface(instance, window);
}

VgeSurface::~VgeSurface()
{
    if (m_surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_instance.getInstance(), m_surface, nullptr);
    }
}

/* Creates a Vulkan surface using GLFW
 *
 * This function creates a window surface using GLFW to interface with the
 * Vulkan instance.
 */
void VgeSurface::createSurface(VgeInstance& instance, VgeWindow& window)
{
    if (glfwCreateWindowSurface(
            instance.getInstance(),
            window.getGLFWwindow(),
            nullptr,
            &m_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface!");
    }
}

/* Get the surface
 *
 * Returns the Vulkan surface associated with the device.
 */
VkSurfaceKHR VgeSurface::getSurface()
{
    return m_surface;
}

} // namespace vge

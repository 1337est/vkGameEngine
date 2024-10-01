// hdrs
#include "vge_surface.hpp"

// libs
// stds

namespace vge
{

VgeSurface::VgeSurface()
{
}

VgeSurface::~VgeSurface()
{
}

/* Creates a Vulkan surface using GLFW
 *
 * This function creates a window surface using GLFW to interface with the
 * Vulkan instance.
 */
void VgeSurface::createSurface()
{
    m_window.createWindowSurface(m_instance, &m_surface);
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

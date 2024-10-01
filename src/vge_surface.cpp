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

VkSurfaceKHR VgeSurface::getSurface() const
{
    return m_surface;
}

} // namespace vge

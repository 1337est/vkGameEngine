#include "vge_surface.hpp"
#include <stdexcept>

namespace vge {

VgeSurface::VgeSurface(VgeInstance& vgeInstance, VgeWindow& vgeWindow)
    : m_vgeInstance{ vgeInstance }
    , m_vgeWindow{ vgeWindow }
    , m_instance{ m_vgeInstance.getInstance() }
    , m_window{ m_vgeWindow.getWindow() }
{
    createSurface();
}

VgeSurface::~VgeSurface()
{
    if (m_surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    }
}

void VgeSurface::createSurface()
{
    if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
}

VkSurfaceKHR VgeSurface::getSurface() const
{
    return m_surface;
}

} // namespace vge

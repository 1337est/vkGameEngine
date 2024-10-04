#include "vge_surface.hpp"
#include <stdexcept>

namespace vge {

VgeSurface::VgeSurface(VkInstance instance, GLFWwindow* window)
    : m_instance{ instance }
    , m_window{ window }
{
    createSurface(instance, window);
}

VgeSurface::~VgeSurface()
{
    if (m_surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    }
}

void VgeSurface::createSurface(VkInstance instance, GLFWwindow* window)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, &m_surface) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
}

VkSurfaceKHR VgeSurface::getSurface() const
{
    return m_surface;
}

} // namespace vge

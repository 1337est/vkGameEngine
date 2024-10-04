#include "vge_surface.hpp"
#include <iostream>
#include <stdexcept>

namespace vge {

VgeSurface::VgeSurface(VkInstance instance, GLFWwindow* window) :
    m_instance{ instance },
    m_window{ window }
{
    std::cout << "START: VgeSurface Constructor\n";
    createSurface(instance, window);
    std::cout << "Vulkan Surface created.\n";
    std::cout << "END: VgeSurface Constructor\n\n";
}

VgeSurface::~VgeSurface()
{
    std::cout << "START: VgeSurface Destructor\n";
    if (m_surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        std::cout << "Vulkan Surface destroyed.\n";
    }
    std::cout << "END: VgeSurface Destructor\n\n";
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

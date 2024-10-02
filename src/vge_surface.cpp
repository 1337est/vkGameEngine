#include "vge_surface.hpp"
#include <iostream>
#include <stdexcept>

namespace vge
{

VgeSurface::VgeSurface(VgeInstance& instance, VgeWindow& window)
    : m_instance{ instance }
    , m_window{ window }
{
    std::cout << "VgeSurface Constructor: Initializing surface creation.\n"
              << "Using Vulkan instance and window context.\n";
    createSurface(instance, window);
    std::cout << "VgeSurface Constructor: Surface successfully created.\n";
}

VgeSurface::~VgeSurface()
{
    if (m_surface != VK_NULL_HANDLE)
    {
        std::cout << "VgeSurface Destructor: Destroying surface." << std::endl;
        vkDestroySurfaceKHR(m_instance.getInstance(), m_surface, nullptr);
        std::cout << "VgeSurface Destructor: Surface destruction complete."
                  << std::endl;
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

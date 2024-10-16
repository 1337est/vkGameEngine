#pragma once
#include "vge_instance.hpp"
#include "vge_window.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeSurface {
public:
    VgeSurface(VgeInstance& vgeInstance, VgeWindow& vgeWindow);
    ~VgeSurface();

    VgeSurface(const VgeSurface&) = delete;
    VgeSurface& operator=(const VgeSurface&) = delete;

    VkSurfaceKHR getSurface() const;

private:
    void createSurface();

    VgeInstance& m_vgeInstance;
    VgeWindow& m_vgeWindow;
    VkInstance m_instance;
    GLFWwindow* m_window;

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
};
} // namespace vge

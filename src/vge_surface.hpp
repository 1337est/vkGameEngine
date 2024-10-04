#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

namespace vge
{
class VgeSurface
{
public:
    VgeSurface(VkInstance instance, GLFWwindow* window);
    ~VgeSurface();

    VgeSurface(const VgeSurface&) = delete;
    VgeSurface& operator=(const VgeSurface&) = delete;

    VkSurfaceKHR getSurface() const;

private:
    void createSurface(VkInstance instance, GLFWwindow* window);

    VkInstance m_instance;
    GLFWwindow* m_window;

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
};
} // namespace vge

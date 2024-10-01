#pragma once
// hdrs
#include "vge_instance.hpp"
#include "vge_window.hpp"
// libs
#include <vulkan/vulkan_core.h>

// stds

namespace vge
{
class VgeSurface
{
public:
    VgeSurface(VgeInstance& instance, VgeWindow& window);
    ~VgeSurface();

    VgeSurface(const VgeSurface&) = delete;
    VgeSurface& operator=(const VgeSurface&) = delete;

    VkSurfaceKHR getSurface();

private:
    void createSurface(VgeInstance& instance, VgeWindow& window);

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VgeInstance& m_instance;
    VgeWindow& m_window;
};
} // namespace vge

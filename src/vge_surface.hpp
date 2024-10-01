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
    VgeSurface();
    ~VgeSurface();

    VgeSurface(const VgeSurface&) = delete;
    VgeSurface& operator=(const VgeSurface&) = delete;

    VkSurfaceKHR getSurface() const;

private:
    void createSurface(const VgeInstance& instance, const VgeWindow& window);

    VkSurfaceKHR m_surface;
};
} // namespace vge

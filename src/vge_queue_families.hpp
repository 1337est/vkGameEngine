#pragma once
#include <vulkan/vulkan_core.h>

namespace vge
{
class VgeQueueFamilies
{
public:
    VgeQueueFamilies() = default;
    VgeQueueFamilies(
        const VkPhysicalDevice& physicalDevice,
        VkSurfaceKHR surface);

    // Not copyable or movable
    VgeQueueFamilies(const VgeQueueFamilies&) = delete;
    VgeQueueFamilies& operator=(const VgeQueueFamilies&) = delete;
    VgeQueueFamilies(VgeQueueFamilies&&) = delete;
    VgeQueueFamilies& operator=(VgeQueueFamilies&&) = delete;

    bool isComplete() const;

    uint32_t getGraphicsFamily() const;
    uint32_t getPresentFamily() const;

private:
    void findQueueFamilies(
        const VkPhysicalDevice& physicalDevice,
        VkSurfaceKHR surface);

    uint32_t m_graphicsFamily = UINT32_MAX;
    bool m_graphicsFamilyHasValue = false;

    uint32_t m_presentFamily = UINT32_MAX;
    bool m_presentFamilyHasValue = false;
};
} // namespace vge

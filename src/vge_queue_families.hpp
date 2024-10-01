#pragma once
// hdrs
// libs
#include <vulkan/vulkan_core.h>

// stds

namespace vge
{
class VgeQueueFamilies
{
public:
    VgeQueueFamilies() = default;
    VgeQueueFamilies(const VkPhysicalDevice& device);

    // Not copyable or movable
    VgeQueueFamilies(const VgeQueueFamilies&) = delete;
    VgeQueueFamilies& operator=(const VgeQueueFamilies&) = delete;
    VgeQueueFamilies(VgeQueueFamilies&&) = delete;
    VgeQueueFamilies& operator=(VgeQueueFamilies&&) = delete;

    bool isComplete() const;

    uint32_t getGraphicsFamily() const;

private:
    void findQueueFamilies(const VkPhysicalDevice& device);

    uint32_t m_graphicsFamily = UINT32_MAX;
    bool m_graphicsFamilyHasValue = false;
};
} // namespace vge

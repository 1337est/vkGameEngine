#include "vge_queue_families.hpp"
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge
{
VgeQueueFamilies::VgeQueueFamilies(
    const VkPhysicalDevice& device,
    VkSurfaceKHR surface)
{
    findQueueFamilies(device, surface);
}

bool VgeQueueFamilies::isComplete() const
{
    return m_graphicsFamilyHasValue && m_presentFamilyHasValue;
}

uint32_t VgeQueueFamilies::getGraphicsFamily() const
{
    return m_graphicsFamily;
}

uint32_t VgeQueueFamilies::getPresentFamily() const
{
    return m_presentFamily;
}

void VgeQueueFamilies::findQueueFamilies(
    const VkPhysicalDevice& device,
    VkSurfaceKHR surface)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        device,
        &queueFamilyCount,
        nullptr);

    if (queueFamilyCount == 0)
    {
        throw std::runtime_error("Failed to find queue families.");
    }

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        device,
        &queueFamilyCount,
        queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilyCount; ++i)
    {
        const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

        // check if queue support graphics operations
        if (queueFamily.queueCount > 0 &&
            queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            m_graphicsFamily = i;
            m_graphicsFamilyHasValue = true;
        }

        // check if queue supports presentation to the surface
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            device,
            i,
            surface,
            &presentSupport);
        if (queueFamily.queueCount > 0 && presentSupport)
        {
            m_presentFamily = i;
            m_presentFamilyHasValue = true;
        }

        if (isComplete())
        {
            break;
        }
    }

    // throw error if no families are found
    if (!m_graphicsFamilyHasValue)
    {
        throw std::runtime_error("Failed to find a graphics queue family.");
    }
    if (!m_presentFamilyHasValue)
    {
        throw std::runtime_error("Failed to find a presentation queue family.");
    }
}
} // namespace vge

// hdrs
#include "vge_queue_families.hpp"
// libs
// stds
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge
{
/* Initializes QueueFamilies and finds queue families
 *
 * This constructor automatically searches for queue families that support
 * graphics operations on the specified physical device.
 */
VgeQueueFamilies::VgeQueueFamilies(
    const VkPhysicalDevice& device,
    VkSurfaceKHR surface)
{
    findQueueFamilies(device, surface);
}

/* Verifies if the necessary queue families for graphics operations
 * have been set. Returns true if the graphics queue family has been found.
 */
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

/* Finds queue families for a physical device that support graphics operations
 *
 * This function queries the physical device for available queue families and
 * checks if any support graphics operations. It updates the internal state
 * of the QueueFamilies object to reflect which queue family supports
 * graphics.
 */
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

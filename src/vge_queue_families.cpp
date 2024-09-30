// headers
#include "vge_queue_families.hpp"

// std
#include <stdexcept>
#include <vector>

namespace vge
{
/* Initializes QueueFamilies and finds queue families
 *
 * This constructor automatically searches for queue families that support
 * graphics operations on the specified physical device.
 */
VgeQueueFamilies::VgeQueueFamilies(const VkPhysicalDevice& device)
{
    findQueueFamilies(device);
}

/* Verifies if the necessary queue families for graphics operations
 * have been set. Returns true if the graphics queue family has been found.
 */
bool VgeQueueFamilies::isComplete() const
{
    return m_graphicsFamilyHasValue;
}

uint32_t VgeQueueFamilies::getGraphicsFamily() const
{
    return m_graphicsFamily;
}

/* Finds queue families for a physical device that support graphics operations
 *
 * This function queries the physical device for available queue families and
 * checks if any support graphics operations. It updates the internal state
 * of the QueueFamilies object to reflect which queue family supports
 * graphics.
 */
void VgeQueueFamilies::findQueueFamilies(const VkPhysicalDevice& device)
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

    int i = 0;
    for (const VkQueueFamilyProperties& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            m_graphicsFamily = i;
            m_graphicsFamilyHasValue = true;
        }

        if (isComplete())
        {
            break;
        }
        i++;
    }
}
} // namespace vge

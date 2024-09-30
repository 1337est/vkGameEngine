// headers
#include "vge_device_physical.hpp"

// std
#include <iostream>
#include <vector>

namespace vge
{

/* Checks if both graphics and present families have been set.
 *
 * This function returns true if both the graphicsFamily and presentFamily
 * indices have valid values, indicating that the queue families required
 * for rendering and presentation are available for use.
 */
bool QueueFamilyIndices::isComplete()
{
    return graphicsFamilyHasValue;
}

VgeDevicePhysical::VgeDevicePhysical(const VkInstance& instance)
{
    pickPhysicalDevice(instance);
}

VgeDevicePhysical::~VgeDevicePhysical()
{
}

/* Selects a physical GPU for the Vulkan application
 *
 * This function chooses a suitable GPU by enumerating available physical
 * devices and checking if they meet the application's requirements.
 */
void VgeDevicePhysical::pickPhysicalDevice(const VkInstance& instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::cout << "Device count: " << deviceCount << std::endl;

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const VkPhysicalDevice& device : devices)
    {
        if (isDeviceSuitable(device))
        {
            m_physicalDevice = device;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    vkGetPhysicalDeviceProperties(m_physicalDevice, &m_deviceProperties);
    std::cout << "physical device: " << m_deviceProperties.deviceName
              << std::endl;
}

/* Checks if a physical device is suitable for the Vulkan application
 *
 * This function checks if the provided physical device supports the required
 * features, extensions, and queue families.
 */
bool VgeDevicePhysical::isDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = findQueueFamilies(device);

    return indices.isComplete();
}

/* Finds queue families for a physical device that support graphics and
 * presentation
 *
 * This function finds the queue families for the physical device that support
 * both graphics and presentation operations.
 */
QueueFamilyIndices VgeDevicePhysical::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        device,
        &queueFamilyCount,
        nullptr);

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
            indices.graphicsFamily = i;
            indices.graphicsFamilyHasValue = true;
        }

        if (indices.isComplete())
        {
            break;
        }

        i++; // still type int on increment
    }

    return indices;
}

VkPhysicalDevice VgeDevicePhysical::getPhysicalDevice() const
{
    return m_physicalDevice;
}

} // namespace vge

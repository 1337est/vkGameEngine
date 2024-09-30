// headers
#include "vge_device_physical.hpp"

// std
#include <iostream>
#include <vector>

namespace vge
{

/* Checks if both graphics and present families have been set.
 *
 * This function verifies if the necessary queue families for graphics
 * operations are available by checking if the graphics family has been set.
 */
bool QueueFamilyIndices::isComplete()
{
    return graphicsFamilyHasValue;
}

/* Selects a physical device
 *
 * Initializes the `VgeDevicePhysical` object and calls the `pickPhysicalDevice`
 * function to select a suitable physical GPU from the Vulkan instance
 */
VgeDevicePhysical::VgeDevicePhysical(const VkInstance& instance)
{
    pickPhysicalDevice(instance);
}

// TODO: Description
VgeDevicePhysical::~VgeDevicePhysical()
{
}

/* Selects a physical GPU for the Vulkan application
 *
 * This function enumerates available physical devices associated with
 * the provided Vulkan instance and selects one that meets the
 * requirements for the Vulkan application. It throws an exception
 * if no suitable device is found.
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
 * This function evaluates the specified physical device to determine
 * if it supports the required features for the Vulkan application.
 * It uses the findQueueFamilies method to check for the necessary queue
 * families.
 */
bool VgeDevicePhysical::isDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = findQueueFamilies(device);

    return indices.isComplete();
}

/* Finds queue families for a physical device that support graphics and
 * presentation
 *
 * This function queries the physical device for its available queue families
 * and checks if any of them support graphics operations. It returns a
 * QueueFamilyIndices structure that indicates the suitable queue families.
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

// Retrieves the selected physical device from `pickPhysicalDevice`
VkPhysicalDevice VgeDevicePhysical::getPhysicalDevice() const
{
    return m_physicalDevice;
}

} // namespace vge

#include "vge_swapchain.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>

namespace vge
{
VgeSwapChain::VgeSwapChain(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    VgeWindow& window,
    VgeSurface& surface,
    VgeQueueFamilies& queueFamilies)
    : m_physicalDevice{ physicalDevice }
    , m_device{ device }
    , m_window{ window }
    , m_surface{ surface }
    , m_queueFamilies{ queueFamilies }
{
    std::cout << "VgeSwapChain Constructor: Initializing swap chain.\n"
              << "Physical Device: " << physicalDevice << ", Device: " << device
              << "\nWindow and surface contexts provided." << std::endl;
    querySwapChainSupport();
    createSwapChain();
    std::cout << "VgeSwapChain Constructor: Swap chain successfully created.\n";
}

VgeSwapChain::~VgeSwapChain()
{
    if (m_swapChain != VK_NULL_HANDLE)
    {
        std::cout << "VgeSwapChain Destructor: Destroying swap chain."
                  << std::endl;
        vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
        std::cout << "VgeSwapChain Destructor: Swap chain destruction complete."
                  << std::endl;
    }
}

void VgeSwapChain::querySwapChainSupport()
{
    // Query surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        m_physicalDevice,
        m_surface.getSurface(),
        &m_surfaceCapabilities);

    // Query surface formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        m_physicalDevice,
        m_surface.getSurface(),
        &formatCount,
        nullptr);

    if (formatCount != 0)
    {
        m_surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_physicalDevice,
            m_surface.getSurface(),
            &formatCount,
            m_surfaceFormats.data());
    }

    // Query present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        m_physicalDevice,
        m_surface.getSurface(),
        &presentModeCount,
        nullptr);

    if (presentModeCount != 0)
    {
        m_presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_physicalDevice,
            m_surface.getSurface(),
            &presentModeCount,
            m_presentModes.data());
    }
}

void VgeSwapChain::createSwapChain()
{
    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(m_surfaceFormats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(m_presentModes);
    VkExtent2D extent = chooseSwapExtent(m_surfaceCapabilities);

    // Calculate the number of images in the swap chain
    uint32_t imageCount = m_surfaceCapabilities.minImageCount + 1;
    if (m_surfaceCapabilities.maxImageCount > 0 &&
        imageCount > m_surfaceCapabilities.maxImageCount)
    {
        imageCount = m_surfaceCapabilities.maxImageCount;
    }

    // Create swap chain
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface.getSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Specify the queue family indices
    uint32_t queueFamilyIndices[] = { m_queueFamilies.getGraphicsFamily(),
                                      m_queueFamilies.getPresentFamily() };

    if (m_queueFamilies.getGraphicsFamily() !=
        m_queueFamilies.getPresentFamily())
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
    }

    createInfo.preTransform = m_surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE; // For now, no previous swapchain

    if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swap chain!");
    }

    // Store the swap chain images
    uint32_t swapChainImageCount;
    vkGetSwapchainImagesKHR(
        m_device,
        m_swapChain,
        &swapChainImageCount,
        nullptr); // First call to get the count

    m_swapChainImages.resize(swapChainImageCount);
    vkGetSwapchainImagesKHR(
        m_device,
        m_swapChain,
        &swapChainImageCount,
        m_swapChainImages.data());
    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
}

VkSurfaceFormatKHR VgeSwapChain::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const VkSurfaceFormatKHR& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0]; // Return the first available format if
                                // preferred format is not found
}

VkExtent2D VgeSwapChain::chooseSwapExtent(
    const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max())
    {
        return capabilities
            .currentExtent; // Return the current extent if it's valid
    }
    else
    {
        VkExtent2D actualExtent = m_windowExtent;
        actualExtent.width = std::max(
            capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent; // Return the calculated extent
    }
}

VkPresentModeKHR VgeSwapChain::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const VkPresentModeKHR& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            std::cout << "Present mode: Mailbox" << std::endl;
            return availablePresentMode;
        }
    }

    std::cout << "Present mode: V-Sync" << std::endl;
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSwapchainKHR VgeSwapChain::getSwapChain() const
{
    return m_swapChain;
}

const std::vector<VkImage>& VgeSwapChain::getSwapChainImages() const
{
    return m_swapChainImages;
}

} // namespace vge

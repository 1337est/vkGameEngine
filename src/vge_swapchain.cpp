#include "vge_swapchain.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>

namespace vge
{
VgeSwapChain::VgeSwapChain(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    uint32_t graphicsFamily,
    uint32_t presentFamily,
    VkDevice logicalDevice,
    GLFWwindow* window)
    : m_physicalDevice{ physicalDevice }
    , m_surface{ surface }
    , m_graphicsFamily{ graphicsFamily }
    , m_presentFamily{ presentFamily }
    , m_logicalDevice{ logicalDevice }
    , m_window{ window }
{
    std::cout << "START: VgeSwapChain Constructor\n";
    querySwapChainSupport();
    std::cout << "Query swap chain support.\n";
    createSwapChain();
    std::cout << "Swap chain created.\n";
    std::cout << "END: VgeSwapChain Constructor\n\n";
}

VgeSwapChain::~VgeSwapChain()
{
    std::cout << "START: VgeSwapChain Destructor\n";
    if (m_swapChain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(m_logicalDevice, m_swapChain, nullptr);
        std::cout << "Swap chain destroyed.\n";
    }
    std::cout << "END: VgeSwapChain Destructor\n\n";
}

void VgeSwapChain::querySwapChainSupport()
{
    // Query surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        m_physicalDevice,
        m_surface,
        &m_surfaceCapabilities);

    // Query surface formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        m_physicalDevice,
        m_surface,
        &formatCount,
        nullptr);

    if (formatCount != 0)
    {
        m_surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_physicalDevice,
            m_surface,
            &formatCount,
            m_surfaceFormats.data());
    }

    // Query present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        m_physicalDevice,
        m_surface,
        &presentModeCount,
        nullptr);

    if (presentModeCount != 0)
    {
        m_presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_physicalDevice,
            m_surface,
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
    createInfo.surface = m_surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Specify the queue family indices
    uint32_t queueFamilyIndices[] = { m_graphicsFamily, m_presentFamily };

    if (m_graphicsFamily != m_presentFamily)
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

    if (vkCreateSwapchainKHR(
            m_logicalDevice,
            &createInfo,
            nullptr,
            &m_swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swap chain!");
    }

    // Store the swap chain images
    uint32_t swapChainImageCount;
    vkGetSwapchainImagesKHR(
        m_logicalDevice,
        m_swapChain,
        &swapChainImageCount,
        nullptr); // First call to get the count

    m_swapChainImages.resize(swapChainImageCount);
    vkGetSwapchainImagesKHR(
        m_logicalDevice,
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

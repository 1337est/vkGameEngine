#include "vge_swapchain.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>

namespace vge {
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
    querySwapChainSupport();
    createSwapChain();
}

VgeSwapChain::~VgeSwapChain()
{
    if (m_swapChain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(m_logicalDevice, m_swapChain, nullptr);
    }
}

void VgeSwapChain::querySwapChainSupport()
{
    // Assigns surface capabilities to m_surfaceCapabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        m_physicalDevice,
        m_surface,
        &m_surfaceCapabilities);

    // 1st retrieves supported surface formats
    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        m_physicalDevice,
        m_surface,
        &surfaceFormatCount,
        nullptr);

    // assign m_surfaceFormats with a list of available formats
    if (surfaceFormatCount != 0) {
        m_surfaceFormats.resize(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_physicalDevice,
            m_surface,
            &surfaceFormatCount,
            m_surfaceFormats.data());
    }

    // 1st retrieves supported present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        m_physicalDevice,
        m_surface,
        &presentModeCount,
        nullptr);

    // assign m_presentModes with a list of available present modes
    if (presentModeCount != 0) {
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

    // Specify the queue family indices
    uint32_t queueFamilyIndices[] = { m_graphicsFamily, m_presentFamily };

    // Create swap chain
    VkSwapchainCreateInfoKHR swapchainCreateInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = m_surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = (m_graphicsFamily != m_presentFamily)
                                ? VK_SHARING_MODE_CONCURRENT
                                : VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = (m_graphicsFamily != m_presentFamily)
                                     ? static_cast<uint32_t>(2)
                                     : static_cast<uint32_t>(0),
        .pQueueFamilyIndices = (m_graphicsFamily != m_presentFamily)
                                   ? queueFamilyIndices
                                   : nullptr,
        .preTransform = m_surfaceCapabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    // stores swapchain inside m_swapChain
    if (vkCreateSwapchainKHR(
            m_logicalDevice,
            &swapchainCreateInfo,
            nullptr,
            &m_swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swap chain!");
    }

    // Retrieves count of swap chain images
    uint32_t swapChainImageCount;
    vkGetSwapchainImagesKHR(
        m_logicalDevice,
        m_swapChain,
        &swapChainImageCount,
        nullptr);

    // assign m_swapChainImages with a list of available swapchain images
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
    for (const VkSurfaceFormatKHR& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0]; // Return the first available format if
                                // preferred format is not found
}

VkPresentModeKHR VgeSwapChain::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const VkPresentModeKHR& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            std::cout << "Present mode: Mailbox" << std::endl;
            return availablePresentMode;
        }
    }

    std::cout << "Present mode: V-Sync" << std::endl;
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VgeSwapChain::chooseSwapExtent(
    const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
    if (surfaceCapabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max())
    {
        return surfaceCapabilities
            .currentExtent; // Return the current extent if it's valid
    }
    else {
        VkExtent2D actualExtent = m_windowExtent;
        actualExtent.width = std::max(
            surfaceCapabilities.minImageExtent.width,
            std::min(
                surfaceCapabilities.maxImageExtent.width,
                actualExtent.width));
        actualExtent.height = std::max(
            surfaceCapabilities.minImageExtent.height,
            std::min(
                surfaceCapabilities.maxImageExtent.height,
                actualExtent.height));

        return actualExtent; // Return the calculated extent
    }
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

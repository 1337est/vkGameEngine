#include "vge_swapchain.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>

namespace vge {
VgeSwapchain::VgeSwapchain(VgeDevice& vgeDevice, VgeSurface& vgeSurface, VgeWindow& vgeWindow)
    : m_vgeDevice{ vgeDevice }
    , m_vgeSurface{ vgeSurface }
    , m_vgeWindow{ vgeWindow }
    , m_pDevice{ m_vgeDevice.getPDevice() }
    , m_surface{ m_vgeSurface.getSurface() }
    , m_graphicsFamily{ m_vgeDevice.getGraphicsFamily() }
    , m_presentFamily{ m_vgeDevice.getPresentFamily() }
    , m_lDevice{ m_vgeDevice.getLDevice() }
    , m_window{ m_vgeWindow.getWindow() }
{
    querySwapchainSupport();

    createSwapchain();
}

VgeSwapchain::VgeSwapchain(
    VgeDevice& vgeDevice,
    VgeSurface& vgeSurface,
    VgeWindow& vgeWindow,
    std::shared_ptr<VgeSwapchain> oldSwapchain)
    : m_vgeDevice{ vgeDevice }
    , m_vgeSurface{ vgeSurface }
    , m_vgeWindow{ vgeWindow }
    , m_pDevice{ m_vgeDevice.getPDevice() }
    , m_surface{ m_vgeSurface.getSurface() }
    , m_graphicsFamily{ m_vgeDevice.getGraphicsFamily() }
    , m_presentFamily{ m_vgeDevice.getPresentFamily() }
    , m_lDevice{ m_vgeDevice.getLDevice() }
    , m_window{ m_vgeWindow.getWindow() }
    , m_oldSwapchain{ oldSwapchain }
{
    querySwapchainSupport();
    createSwapchain();

    // cleanup old swapchain since it's no longer needed
    m_oldSwapchain = nullptr;
}

VgeSwapchain::~VgeSwapchain()
{
    if (m_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(m_lDevice, m_swapchain, nullptr);
    }
}

void VgeSwapchain::querySwapchainSupport()
{
    // Assigns surface capabilities to m_surfaceCapabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pDevice, m_surface, &m_surfaceCaps);

    // 1st retrieves supported surface formats
    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_pDevice, m_surface, &surfaceFormatCount, nullptr);

    // assign m_surfaceFormats with a list of available formats
    if (surfaceFormatCount != 0) {
        m_surfaceFormats.resize(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_pDevice,
            m_surface,
            &surfaceFormatCount,
            m_surfaceFormats.data());
    }

    // 1st retrieves supported present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_pDevice, m_surface, &presentModeCount, nullptr);

    // assign m_presentModes with a list of available present modes
    if (presentModeCount != 0) {
        m_presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_pDevice,
            m_surface,
            &presentModeCount,
            m_presentModes.data());
    }
}

void VgeSwapchain::createSwapchain()
{
    VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(m_surfaceFormats);
    VkPresentModeKHR presentMode = choosePresentMode(m_presentModes);
    VkExtent2D extent = chooseExtent(m_surfaceCaps);

    // Calculate the number of images in the swap chain
    uint32_t imageCount = m_surfaceCaps.minImageCount + 1;
    if (m_surfaceCaps.maxImageCount > 0 && imageCount > m_surfaceCaps.maxImageCount) {
        imageCount = m_surfaceCaps.maxImageCount;
    }

    // Specify the queue family indices
    uint32_t queueFamilyIndices[] = {
        m_graphicsFamily,
        m_presentFamily,
    };

    // Create swap chain
    VkSwapchainCreateInfoKHR swapchainCI{
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
        .imageSharingMode = (m_graphicsFamily != m_presentFamily) ? VK_SHARING_MODE_CONCURRENT
                                                                  : VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = (m_graphicsFamily != m_presentFamily) ? static_cast<uint32_t>(2)
                                                                       : static_cast<uint32_t>(0),
        .pQueueFamilyIndices = (m_graphicsFamily != m_presentFamily) ? queueFamilyIndices : nullptr,
        .preTransform = m_surfaceCaps.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = m_oldSwapchain == nullptr ? VK_NULL_HANDLE : m_oldSwapchain->m_swapchain,
    };

    // stores swapchain inside m_swapchain
    if (vkCreateSwapchainKHR(m_lDevice, &swapchainCI, nullptr, &m_swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swap chain!");
    }

    // Retrieves count of swap chain images
    uint32_t swapchainImageCount;
    vkGetSwapchainImagesKHR(m_lDevice, m_swapchain, &swapchainImageCount, nullptr);

    // assign m_swapchainImages with a list of available swapchain images
    m_swapchainImages.resize(swapchainImageCount);
    vkGetSwapchainImagesKHR(m_lDevice, m_swapchain, &swapchainImageCount, m_swapchainImages.data());
    m_swapchainImageFormat = surfaceFormat.format;
    m_swapchainExtent = extent;
}

VkSurfaceFormatKHR VgeSwapchain::chooseSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& surfaceFormats)
{
    for (const VkSurfaceFormatKHR& availableFormat : surfaceFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return surfaceFormats[0]; // Return the first available format if
                              // preferred format is not found
}

VkPresentModeKHR VgeSwapchain::choosePresentMode(const std::vector<VkPresentModeKHR>& presentModes)
{
    for (const VkPresentModeKHR& availablePresentMode : presentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            std::cout << "Present mode: Mailbox" << std::endl;
            return availablePresentMode;
        }
    }

    std::cout << "Present mode: V-Sync" << std::endl;
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VgeSwapchain::chooseExtent(const VkSurfaceCapabilitiesKHR& surfaceCaps)
{
    if (surfaceCaps.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return surfaceCaps.currentExtent; // Return the current extent if it's valid
    }
    else {
        VkExtent2D actualExtent = m_windowExtent;
        actualExtent.width = std::max(
            surfaceCaps.minImageExtent.width,
            std::min(surfaceCaps.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(
            surfaceCaps.minImageExtent.height,
            std::min(surfaceCaps.maxImageExtent.height, actualExtent.height));

        return actualExtent; // Return the calculated extent
    }
}

VkSwapchainKHR VgeSwapchain::getSwapchain() const
{
    return m_swapchain;
}

const std::vector<VkImage>& VgeSwapchain::getSwapchainImages() const
{
    return m_swapchainImages;
}

VkFormat VgeSwapchain::getSwapchainImageFormat() const
{
    return m_swapchainImageFormat;
}

VkExtent2D VgeSwapchain::getSwapchainExtent() const
{
    return m_swapchainExtent;
}
} // namespace vge

#include "vge_image_views.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vge {
VgeImageView::VgeImageView(
    VkDevice lDevice,
    std::vector<VkImage> swapchainImages,
    VkFormat swapchainImageFormat)
    : m_lDevice{ lDevice }
    , m_swapchainImages{ swapchainImages }
    , m_swapchainImageFormat{ swapchainImageFormat }
{
    createImageViews();
}

VgeImageView::~VgeImageView()
{
    for (VkImageView imageView : m_swapchainImageViews) {
        vkDestroyImageView(m_lDevice, imageView, nullptr);
    }
}

void VgeImageView::createImageViews()
{
    m_swapchainImageViews.resize(m_swapchainImages.size());
    for (size_t i = 0; i < m_swapchainImages.size(); i++) {
        VkImageViewCreateInfo imageViewCI{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = m_swapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = m_swapchainImageFormat,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        if (vkCreateImageView(m_lDevice, &imageViewCI, nullptr, &m_swapchainImageViews[i]) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image view!");
        }
    }
}

std::vector<VkImageView> VgeImageView::getSwapchainImageViews()
{
    return m_swapchainImageViews;
}
} // namespace vge

#include "vge_image_views.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vge {
VgeImageView::VgeImageView(VgeDevice& vgeDevice, VgeSwapchain& vgeSwapchain)
    : m_vgeDevice{ vgeDevice }
    , m_vgeSwapchain{ vgeSwapchain }
    , m_lDevice{ m_vgeDevice.getLDevice() }
    , m_images{ m_vgeSwapchain.getSwapchainImages() }
    , m_imageFormat{ m_vgeSwapchain.getSwapchainImageFormat() }
{
    createImageViews();
}

VgeImageView::~VgeImageView()
{
    for (VkImageView imageView : m_imageViews) {
        vkDestroyImageView(m_lDevice, imageView, nullptr);
    }
}

void VgeImageView::createImageViews()
{
    m_imageViews.resize(m_images.size());
    for (size_t i = 0; i < m_images.size(); i++) {
        VkImageViewCreateInfo imageViewCI{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = m_images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = m_imageFormat,
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

        if (vkCreateImageView(m_lDevice, &imageViewCI, nullptr, &m_imageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image view!");
        }
    }
}

std::vector<VkImageView> VgeImageView::getImageViews()
{
    return m_imageViews;
}
} // namespace vge

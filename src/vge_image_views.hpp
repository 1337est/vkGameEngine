#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeImageView {
public:
    VgeImageView(VkDevice lDevice, std::vector<VkImage> swapchainImages, VkFormat swapchainImageFormat);
    ~VgeImageView();
    VgeImageView(const VgeImageView&) = delete;
    VgeImageView& operator=(const VgeImageView&) = delete;

private:
    void createImageViews();

    VkDevice m_lDevice = VK_NULL_HANDLE;

    std::vector<VkImage> m_swapchainImages;
    VkFormat m_swapchainImageFormat;

    std::vector<VkImageView> m_swapchainImageViews;
};
} // namespace vge

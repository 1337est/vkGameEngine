#pragma once

#include "vge_device.hpp"
#include "vge_swapchain.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeImageView {
public:
    VgeImageView(VgeDevice& vgeDevice, VgeSwapchain& vgeSwapchain);
    ~VgeImageView();
    VgeImageView(const VgeImageView&) = delete;
    VgeImageView& operator=(const VgeImageView&) = delete;

    size_t getImages();
    std::vector<VkImageView> getImageViews();

private:
    void createImageViews();

    VgeDevice& m_vgeDevice;
    VgeSwapchain& m_vgeSwapchain;

    VkDevice m_lDevice = VK_NULL_HANDLE;
    std::vector<VkImage> m_images;
    VkFormat m_imageFormat;

    std::vector<VkImageView> m_imageViews; // yes
};
} // namespace vge

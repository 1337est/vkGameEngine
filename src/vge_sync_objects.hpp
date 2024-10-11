#pragma once

#include <vulkan/vulkan_core.h>

namespace vge {
class VgeSyncObjects {
public:
    VgeSyncObjects(VkDevice lDevice);
    ~VgeSyncObjects();
    VgeSyncObjects(const VgeSyncObjects&) = delete;
    VgeSyncObjects& operator=(const VgeSyncObjects&) = delete;

    VkSemaphore getImageAvailableSemaphore() const;
    VkSemaphore getRenderFinishedSemaphore() const;
    VkFence getInFlightFence() const;

private:
    void createSyncObjects();

    VkDevice m_lDevice;

    VkSemaphore m_imageAvailableSemaphore;
    VkSemaphore m_renderFinishedSemaphore;
    VkFence m_inFlightFence;
};
} // namespace vge

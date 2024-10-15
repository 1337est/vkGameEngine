#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeSyncObjects {
public:
    VgeSyncObjects(VkDevice lDevice);
    ~VgeSyncObjects();
    VgeSyncObjects(const VgeSyncObjects&) = delete;
    VgeSyncObjects& operator=(const VgeSyncObjects&) = delete;

    std::vector<VkSemaphore> getImageAvailableSemaphore() const;
    std::vector<VkSemaphore> getRenderFinishedSemaphore() const;
    std::vector<VkFence> getInFlightFence() const;

private:
    void createSyncObjects();

    VkDevice m_lDevice;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
};
} // namespace vge

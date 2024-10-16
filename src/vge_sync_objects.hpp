#pragma once

#include "vge_device.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeSyncObjects {
public:
    VgeSyncObjects(VgeDevice& vgeDevice);
    ~VgeSyncObjects();
    VgeSyncObjects(const VgeSyncObjects&) = delete;
    VgeSyncObjects& operator=(const VgeSyncObjects&) = delete;

    std::vector<VkSemaphore> getImageAvailableSemaphore() const;
    std::vector<VkSemaphore> getRenderFinishedSemaphore() const;
    std::vector<VkFence> getInFlightFence() const;

private:
    void createSyncObjects();

    VgeDevice& m_vgeDevice;

    VkDevice m_lDevice;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
};
} // namespace vge

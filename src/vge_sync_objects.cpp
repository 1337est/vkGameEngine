#include "vge_sync_objects.hpp"
#include <stdexcept>

namespace vge {
VgeSyncObjects::VgeSyncObjects(VkDevice lDevice)
    : m_lDevice{ lDevice }
{
    createSyncObjects();
}

VgeSyncObjects::~VgeSyncObjects()
{
    vkDestroySemaphore(m_lDevice, m_imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(m_lDevice, m_renderFinishedSemaphore, nullptr);
    vkDestroyFence(m_lDevice, m_inFlightFence, nullptr);
}

void VgeSyncObjects::createSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(m_lDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(m_lDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(m_lDevice, &fenceInfo, nullptr, &m_inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
}
} // namespace vge

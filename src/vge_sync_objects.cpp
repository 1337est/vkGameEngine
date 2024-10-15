#include "vge_sync_objects.hpp"
#include "max_frames_in_flight.hpp"
#include <stdexcept>

namespace vge {
VgeSyncObjects::VgeSyncObjects(VkDevice lDevice)
    : m_lDevice{ lDevice }
{
    createSyncObjects();
}

VgeSyncObjects::~VgeSyncObjects()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(m_lDevice, m_imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(m_lDevice, m_renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(m_lDevice, m_inFlightFences[i], nullptr);
    }
}

void VgeSyncObjects::createSyncObjects()
{
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    VkFenceCreateInfo fenceInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,

    };

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(m_lDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_lDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_lDevice, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

std::vector<VkSemaphore> VgeSyncObjects::getImageAvailableSemaphore() const
{
    return m_imageAvailableSemaphores;
}

std::vector<VkSemaphore> VgeSyncObjects::getRenderFinishedSemaphore() const
{
    return m_renderFinishedSemaphores;
}

std::vector<VkFence> VgeSyncObjects::getInFlightFence() const
{
    return m_inFlightFences;
}
} // namespace vge

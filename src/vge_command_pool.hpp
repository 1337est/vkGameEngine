#pragma once

#include "vge_device.hpp"
#include <vulkan/vulkan_core.h>

namespace vge {
class VgeCommandPool {
public:
    VgeCommandPool(VgeDevice& vgeDevice);
    ~VgeCommandPool();
    VgeCommandPool(const VgeCommandPool&) = delete;
    VgeCommandPool& operator=(const VgeCommandPool&) = delete;

    VkCommandPool getCommandPool() const;

private:
    void createCommandPool();

    VgeDevice& m_vgeDevice;

    VkDevice m_lDevice;
    uint32_t m_graphicsFamily;

    VkCommandPool m_commandPool;
};
} // namespace vge

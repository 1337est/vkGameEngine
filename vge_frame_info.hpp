#pragma once

#include "vge_camera.hpp"

// libs
#include <vulkan/vulkan.h>

namespace vge
{
struct FrameInfo
{
    int frameIndex{};
    float frameTime{};
    VkCommandBuffer commandBuffer{};
    VgeCamera& camera;
};
} // namespace vge

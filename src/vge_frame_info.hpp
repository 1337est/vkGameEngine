#pragma once

#include "vge_camera.hpp"
#include "vge_game_object.hpp"

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
    VkDescriptorSet globalDescriptorSet;
    VgeGameObject::Map& gameObjects;
};
} // namespace vge

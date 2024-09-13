#pragma once

// headers
#include "vge_camera.hpp"
#include "vge_game_object.hpp"

// libs
#include <vulkan/vulkan.h>

namespace vge
{

#define MAX_LIGHTS 10

struct PointLight
{
    glm::vec4 position{}; // ignore w
    glm::vec4 color{};    // w is intensity
};

struct GlobalUbo // Uniform buffer object
{
    glm::mat4 projection{ 1.f };
    glm::mat4 view{ 1.f };
    glm::mat4 inverseView{ 1.f };

    glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f }; // w is intensity
    PointLight pointLights[MAX_LIGHTS];
    int numLights;
};

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

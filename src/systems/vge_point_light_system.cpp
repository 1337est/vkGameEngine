// headers
#include "vge_point_light_system.hpp"
#include <glm/ext/vector_float4.hpp>
#include <vulkan/vulkan_core.h>

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace vge
{

struct PointLightPushConstants
{
    glm::vec4 position{};
    glm::vec4 color{};
    float radius;
};

VgePointLightSystem::VgePointLightSystem(
    VgeDevice& device,
    VkRenderPass renderPass,
    VkDescriptorSetLayout globalSetLayout)
    : m_vgeDevice{ device }
    , m_vgePipeline{}
    , m_pipelineLayout{}
{
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

VgePointLightSystem::~VgePointLightSystem()
{
    vkDestroyPipelineLayout(m_vgeDevice.device(), m_pipelineLayout, nullptr);
}

void VgePointLightSystem::createPipelineLayout(
    VkDescriptorSetLayout globalSetLayout)
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PointLightPushConstants);

    // TODO: for having multiple sets (currently only set 0, so not needed yet)
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount =
        static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(
            m_vgeDevice.device(),
            &pipelineLayoutInfo,
            nullptr,
            &m_pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout!");
    }
}

void VgePointLightSystem::createPipeline(VkRenderPass renderPass)
{
    assert(
        m_pipelineLayout != nullptr &&
        "Cannot create pipeline before pipeline layout!");

    PipelineConfigInfo pipelineConfig{};
    VgePipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.attributeDescriptions.clear();
    pipelineConfig.bindingDescriptions.clear();
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = m_pipelineLayout;
    m_vgePipeline = std::make_unique<VgePipeline>(
        m_vgeDevice,
        "shaders/point_light.vert.spv",
        "shaders/point_light.frag.spv",
        pipelineConfig);
}

void VgePointLightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo)
{
    // rotate lights
    auto rotateLight =
        glm::rotate(glm::mat4(1.f), frameInfo.frameTime, { 0.f, -1.f, 0.f });

    int lightIndex = 0;
    for (auto& kv : frameInfo.gameObjects)
    {
        auto& obj = kv.second;
        if (obj.m_pointLight == nullptr)
            continue;

        assert(
            lightIndex < MAX_LIGHTS &&
            "Point lights exceed maximum specified!");

        // update light postion
        obj.m_transform.translation = glm::vec3(
            rotateLight * glm::vec4(obj.m_transform.translation, 1.f));

        // copy light to ubo
        ubo.pointLights[lightIndex].position =
            glm::vec4(obj.m_transform.translation, 1.f);
        ubo.pointLights[lightIndex].color =
            glm::vec4(obj.m_color, obj.m_pointLight->lightIntensity);

        lightIndex += 1;
    }
    ubo.numLights = lightIndex;
}

void VgePointLightSystem::render(FrameInfo& frameInfo)
{
    m_vgePipeline->bind(frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipelineLayout,
        0,
        1,
        &frameInfo.globalDescriptorSet,
        0,
        nullptr);

    for (auto& kv : frameInfo.gameObjects)
    {
        auto& obj = kv.second;
        if (obj.m_pointLight == nullptr)
            continue;

        PointLightPushConstants push{};
        push.position = glm::vec4(obj.m_transform.translation, 1.f);
        push.color = glm::vec4(obj.m_color, obj.m_pointLight->lightIntensity);
        push.radius = obj.m_transform.scale.x;

        vkCmdPushConstants(
            frameInfo.commandBuffer,
            m_pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(PointLightPushConstants),
            &push);

        vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }
}

} // namespace vge

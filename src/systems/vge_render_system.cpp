// headers
#include "vge_render_system.hpp"
#include "vge_game_object.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <cassert>
#include <stdexcept>

namespace vge
{

struct SimplePushConstantData
{
    glm::mat4 modelMatrix{ 1.f };  // identity matrix
    glm::mat4 normalMatrix{ 1.f }; // identity matrix
};

VgeRenderSystem::VgeRenderSystem(
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

VgeRenderSystem::~VgeRenderSystem()
{
    vkDestroyPipelineLayout(m_vgeDevice.device(), m_pipelineLayout, nullptr);
}

void VgeRenderSystem::createPipelineLayout(
    VkDescriptorSetLayout globalSetLayout)
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

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

void VgeRenderSystem::createPipeline(VkRenderPass renderPass)
{
    assert(
        m_pipelineLayout != nullptr &&
        "Cannot create pipeline before pipeline layout!");

    PipelineConfigInfo pipelineConfig{};
    VgePipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = m_pipelineLayout;
    m_vgePipeline = std::make_unique<VgePipeline>(
        m_vgeDevice,
        "./shaders/shader.vert.spv",
        "./shaders/shader.frag.spv",
        pipelineConfig);
}

void VgeRenderSystem::renderGameObjects(FrameInfo& frameInfo)
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
        // kv.second = gameObj kv.first = objId
        auto& obj = kv.second;
        if (obj.m_model == nullptr)
            continue;
        SimplePushConstantData pushData{};
        pushData.modelMatrix = obj.m_transform.mat4();
        pushData.normalMatrix = obj.m_transform.normalMatrix();

        vkCmdPushConstants(
            frameInfo.commandBuffer,
            m_pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &pushData);
        obj.m_model->bind(frameInfo.commandBuffer);
        obj.m_model->draw(frameInfo.commandBuffer);
    }
}

} // namespace vge

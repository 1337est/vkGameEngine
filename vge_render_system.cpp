// headers
#include "vge_render_system.hpp"
#include "vge_game_object.hpp"

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

struct SimplePushConstantData
{
    glm::mat4 m_transform{ 1.f };  // identity matrix
    glm::mat4 normalMatrix{ 1.f }; // identity matrix
};

VgeRenderSystem::VgeRenderSystem(VgeDevice& device, VkRenderPass renderPass)
    : m_vgeDevice{ device }
    , m_vgePipeline{}
    , m_pipelineLayout{}
{
    createPipelineLayout();
    createPipeline(renderPass);
}

VgeRenderSystem::~VgeRenderSystem()
{
    vkDestroyPipelineLayout(m_vgeDevice.device(), m_pipelineLayout, nullptr);
}

void VgeRenderSystem::createPipelineLayout()
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
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

void VgeRenderSystem::renderGameObjects(
    FrameInfo& frameInfo,
    std::vector<VgeGameObject>& gameObjects)
{
    m_vgePipeline->bind(frameInfo.commandBuffer);

    auto projectionView =
        frameInfo.camera.getProjection() * frameInfo.camera.getView();

    for (auto& obj : gameObjects)
    {
        SimplePushConstantData pushData{};
        auto modelMatrix = obj.m_transform.mat4();
        pushData.m_transform = projectionView * modelMatrix;
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

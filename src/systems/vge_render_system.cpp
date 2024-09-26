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

/* Constructs a VgeRenderSystem object.
 *
 * Initializes the render system by creating the pipeline layout and
 * pipeline with the provided Vulkan device, render pass, and global
 * descriptor set layout.
 */
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

/* Destroys the VgeRenderSystem object.
 *
 * Cleans up the Vulkan pipeline layout used by the render system
 * to release resources.
 */
VgeRenderSystem::~VgeRenderSystem()
{
    vkDestroyPipelineLayout(m_vgeDevice.device(), m_pipelineLayout, nullptr);
}

/* Creates the pipeline layout for the render system.
 *
 * Sets up the push constant range and descriptor set layouts for the
 * pipeline, allowing the shader to access model transformation data.
 */
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

/* Creates the graphics pipeline for rendering game objects.
 *
 * Configures the pipeline settings, including vertex and fragment shader
 * paths, and binds it to the specified render pass.
 */
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

/* Renders game objects in the current frame.
 *
 * Binds the pipeline and descriptor sets, then pushes the transformation
 * matrices for each game object to the shaders and issues draw calls
 * for the corresponding models.
 */
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

    for (std::pair<const unsigned int, VgeGameObject>& kv :
         frameInfo.gameObjects)
    {
        // kv.second = gameObj kv.first = objId
        VgeGameObject& obj = kv.second;
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

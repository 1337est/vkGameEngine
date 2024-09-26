#pragma once

// headers
#include "vge_device.hpp"
#include "vge_frame_info.hpp"
#include "vge_pipeline.hpp"

// libs
#include <vulkan/vulkan_core.h>

// std
#include <memory>

namespace vge
{

struct SimplePushConstantData
{
    glm::mat4 modelMatrix{ 1.f };  // identity matrix
    glm::mat4 normalMatrix{ 1.f }; // identity matrix
};

class VgeRenderSystem
{
public:
    VgeRenderSystem(
        VgeDevice& device,
        VkRenderPass renderPass,
        VkDescriptorSetLayout globalSetLayout);
    ~VgeRenderSystem();

    VgeRenderSystem(const VgeRenderSystem&) = delete;
    VgeRenderSystem& operator=(const VgeRenderSystem&) = delete;

    void renderGameObjects(FrameInfo& frameInfo);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    VgeDevice& m_vgeDevice; // use device for window
    std::unique_ptr<VgePipeline> m_vgePipeline;
    VkPipelineLayout m_pipelineLayout;
};

} // namespace vge

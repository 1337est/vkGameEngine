#pragma once

#include "../vge_device.hpp"
#include "../vge_frame_info.hpp"
#include "../vge_pipeline.hpp"

#include <vulkan/vulkan_core.h>

#include <memory>

namespace vge {

struct PointLightPushConstants
{
    glm::vec4 position{};
    glm::vec4 color{};
    float radius;
};

class VgePointLightSystem {
public:
    VgePointLightSystem(
        VgeDevice& device,
        VkRenderPass renderPass,
        VkDescriptorSetLayout globalSetLayout);
    ~VgePointLightSystem();

    VgePointLightSystem(const VgePointLightSystem&) = delete;
    VgePointLightSystem& operator=(const VgePointLightSystem&) = delete;

    void update(FrameInfo& frameInfo, GlobalUbo& ubo);
    void render(FrameInfo& frameInfo);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    VgeDevice& m_vgeDevice; // use device for window
    std::unique_ptr<VgePipeline> m_vgePipeline;
    VkPipelineLayout m_pipelineLayout;
};

} // namespace vge

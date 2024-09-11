#pragma once

// headers
#include "../vge_camera.hpp"
#include "../vge_device.hpp"
#include "../vge_frame_info.hpp"
#include "../vge_game_object.hpp"
#include "../vge_pipeline.hpp"

// libs
#include <vulkan/vulkan_core.h>

// std
#include <memory>
#include <vector>

namespace vge
{

class VgePointLightSystem
{
public:
    VgePointLightSystem(
        VgeDevice& device,
        VkRenderPass renderPass,
        VkDescriptorSetLayout globalSetLayout);
    ~VgePointLightSystem();

    VgePointLightSystem(const VgePointLightSystem&) = delete;
    VgePointLightSystem& operator=(const VgePointLightSystem&) = delete;

    void render(FrameInfo& frameInfo);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    VgeDevice& m_vgeDevice; // use device for window
    std::unique_ptr<VgePipeline> m_vgePipeline;
    VkPipelineLayout m_pipelineLayout;
};

} // namespace vge

#pragma once

// headers
#include "vge_camera.hpp"
#include "vge_device.hpp"
#include "vge_frame_info.hpp"
#include "vge_game_object.hpp"
#include "vge_pipeline.hpp"

// libs
#include <vulkan/vulkan_core.h>

// std
#include <memory>
#include <vector>

namespace vge
{

class VgeRenderSystem
{
public:
    VgeRenderSystem(VgeDevice& device, VkRenderPass renderPass);
    ~VgeRenderSystem();

    VgeRenderSystem(const VgeRenderSystem&) = delete;
    VgeRenderSystem& operator=(const VgeRenderSystem&) = delete;

    void renderGameObjects(
        FrameInfo& frameInfo,
        std::vector<VgeGameObject>& gameObjects);

private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

    VgeDevice& m_vgeDevice; // use device for window
    std::unique_ptr<VgePipeline> m_vgePipeline;
    VkPipelineLayout m_pipelineLayout;
};

} // namespace vge

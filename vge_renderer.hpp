#pragma once

// headers
#include "vge_device.hpp"
#include "vge_swapchain.hpp"
#include "vge_window.hpp"

// libraries
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

// std
#include <cassert>
#include <memory>
#include <vector>

namespace vge
{

class VgeRenderer
{
public:
    VgeRenderer(VgeWindow& window, VgeDevice& device);
    ~VgeRenderer();

    VgeRenderer(const VgeRenderer&) = delete;
    VgeRenderer& operator=(const VgeRenderer&) = delete;

    VkRenderPass getSwapChainRenderPass() const
    {
        return m_vgeSwapChain->getRenderPass();
    }

    float getAspectRatio() const
    {
        return m_vgeSwapChain->extentAspectRatio();
    }

    bool isFrameInProgress() const
    {
        return m_isFrameStarted;
    }

    VkCommandBuffer getCurrentCommandBuffer() const
    {
        assert(
            m_isFrameStarted &&
            "Cannot get command buffer when frame not in progress");
        return m_commandBuffers[m_currentFrameIndex];
    }

    uint32_t getFrameIndex() const
    {
        assert(
            m_isFrameStarted &&
            "Cannot get frame index when frame not in progress");
        return m_currentFrameIndex;
    }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    VgeWindow& m_vgeWindow; //  window
    VgeDevice& m_vgeDevice; // use device for window
    std::unique_ptr<VgeSwapChain> m_vgeSwapChain;
    std::vector<VkCommandBuffer> m_commandBuffers;

    uint32_t m_currentImageIndex;
    uint32_t m_currentFrameIndex{ 0 };
    bool m_isFrameStarted{ false };
};

} // namespace vge

/*
// Sierpinski exercise: draws recursive triangle
void sierpinski(
    std::vector<VgeModel::Vertex>& vertices,
    int cuts,
    glm::vec2 a,
    glm::vec2 b,
    glm::vec2 c,
    glm::vec3 color);
*/

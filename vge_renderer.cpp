// headers
#include "vge_renderer.hpp"
#include "vge_device.hpp"

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace vge
{

VgeRenderer::VgeRenderer(VgeWindow& window, VgeDevice& device)
    : m_vgeWindow{ window }
    , m_vgeDevice{ device }
    , m_vgeSwapChain{}
    , m_commandBuffers{}
    , m_currentImageIndex{}
    , m_isFrameStarted{}
{
    recreateSwapChain();
    createCommandBuffers();
}

VgeRenderer::~VgeRenderer()
{
    freeCommandBuffers();
}

void VgeRenderer::recreateSwapChain()
{
    auto extent = m_vgeWindow.getExtent();
    while (extent.width == 0 || extent.height == 0)
    {
        extent = m_vgeWindow.getExtent();
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(m_vgeDevice.device());

    if (m_vgeSwapChain == nullptr)
    {
        m_vgeSwapChain = std::make_unique<VgeSwapChain>(m_vgeDevice, extent);
    }
    else
    {
        std::shared_ptr<VgeSwapChain> oldSwapChain = std::move(m_vgeSwapChain);
        m_vgeSwapChain =
            std::make_unique<VgeSwapChain>(m_vgeDevice, extent, oldSwapChain);

        if (!oldSwapChain->compareSwapFormats(*m_vgeSwapChain.get()))
        {
            throw std::runtime_error(
                "Swap chain image(or depth) format has changed!");
        }
    }
}

void VgeRenderer::createCommandBuffers()
{
    m_commandBuffers.resize(VgeSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_vgeDevice.getCommandPool();
    allocInfo.commandBufferCount =
        static_cast<uint32_t>(m_commandBuffers.size());

    if (vkAllocateCommandBuffers(
            m_vgeDevice.device(),
            &allocInfo,
            m_commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffersf!");
    }
}

void VgeRenderer::freeCommandBuffers()
{
    vkFreeCommandBuffers(
        m_vgeDevice.device(),
        m_vgeDevice.getCommandPool(),
        static_cast<uint32_t>(m_commandBuffers.size()),
        m_commandBuffers.data());

    m_commandBuffers.clear();
}

VkCommandBuffer VgeRenderer::beginFrame()
{
    assert(
        !m_isFrameStarted &&
        "Can't call beginFrame while already in progress!");

    auto result = m_vgeSwapChain->acquireNextImage(&m_currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return nullptr;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    m_isFrameStarted = true;

    auto commandBuffer = getCurrentCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    return commandBuffer;
}

void VgeRenderer::endFrame()
{
    assert(
        m_isFrameStarted && "Can't call endFrame while frame not in progress!");

    auto commandBuffer = getCurrentCommandBuffer();

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
    auto result = m_vgeSwapChain->submitCommandBuffers(
        &commandBuffer,
        &m_currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        m_vgeWindow.wasWindowResized())
    {
        m_vgeWindow.resetWindowResizedFlag();
        recreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    m_isFrameStarted = false;
    m_currentFrameIndex =
        (m_currentFrameIndex + 1) % VgeSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void VgeRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    assert(
        m_isFrameStarted &&
        "Can't call beginSwapChainRenderPass if frame is not in progress!");
    assert(
        commandBuffer == getCurrentCommandBuffer() &&
        "Can't begin render pass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_vgeSwapChain->getRenderPass();
    renderPassInfo.framebuffer =
        m_vgeSwapChain->getFrameBuffer(m_currentImageIndex);

    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_vgeSwapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f }; // index 0 is color
    clearValues[1].depthStencil = { 1.0f, 0 };            // index 1 is depth
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());

    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(
        commandBuffer,
        &renderPassInfo,
        VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width =
        static_cast<float>(m_vgeSwapChain->getSwapChainExtent().width);
    viewport.height =
        static_cast<float>(m_vgeSwapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{
        { 0, 0 },
        m_vgeSwapChain->getSwapChainExtent()
    };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void VgeRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    assert(
        m_isFrameStarted &&
        "Can't call endSwapChainRenderPass if frame is not in progress!");
    assert(
        commandBuffer == getCurrentCommandBuffer() &&
        "Can't end render pass on command buffer from a different frame");
    vkCmdEndRenderPass(commandBuffer);
}

} // namespace vge

/*
// Sierpinski exercise: draws recursive triangle
void VgeRenderer::sierpinski(
    std::vector<VgeModel::Vertex>& vertices,
    int cuts,
    glm::vec2 a,
    glm::vec2 b,
    glm::vec2 c,
    [[maybe_unused]] glm::vec3 RGBColor)
{
    glm::vec3 red = { 1.0f, 0.0f, 0.0f };
    glm::vec3 green = { 0.0f, 1.0f, 0.0f };
    glm::vec3 blue = { 0.0f, 0.0f, 1.0f };

    if (cuts <= 0)
    {
        vertices.push_back({ a, RGBColor });
        vertices.push_back({ b, RGBColor });
        vertices.push_back({ c, RGBColor });
    }
    else
    {
        auto ab = 0.5f * (a + b);
        auto ac = 0.5f * (a + c);
        auto bc = 0.5f * (b + c);
        // colored like the LOZ triforce (colors counterclockwise)
        sierpinski(vertices, cuts - 1, a, ab, ac, red);   // top triangle
        sierpinski(vertices, cuts - 1, ab, b, bc, green); // right triangle
        sierpinski(vertices, cuts - 1, ac, bc, c, blue);  // left triangle
    }
}

void VgeRenderer::loadModels()
{
    std::vector<VgeModel::Vertex> vertices{};
    glm::vec3 vertColor{};
    sierpinski(
        vertices,
        2,
        { 0.0f, -0.5f },
        { 0.5f, 0.5f },
        { -0.5f, 0.5f },
        vertColor);
    m_vgeModel = std::make_unique<VgeModel>(m_vgeDevice, vertices);
}
*/

// headers
#include "vge_renderer.hpp"
#include "vge_device.hpp"

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace vge {
/* Initializes a VgeRenderer instance.
 *
 * This constructor takes a reference to a VgeWindow and a VgeDevice,
 * setting up the renderer and initializing necessary resources.
 */
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

/* Cleans up the VgeRenderer instance.
 *
 * This destructor frees any allocated command buffers to ensure
 * proper resource management when the renderer is destroyed.
 */
VgeRenderer::~VgeRenderer()
{
    freeCommandBuffers();
}

/* Recreates the swap chain.
 *
 * This function checks the window's extent and recreates the swap
 * chain if necessary. It also handles the case where the format of
 * the swap chain has changed, throwing an exception if that occurs.
 */
void VgeRenderer::recreateSwapChain()
{
    VkExtent2D extent = m_vgeWindow.getExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = m_vgeWindow.getExtent();
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(m_vgeDevice.getDevice());

    if (m_vgeSwapChain == nullptr) {
        m_vgeSwapChain = std::make_unique<VgeSwapChain>(m_vgeDevice, extent);
    }
    else {
        std::shared_ptr<VgeSwapChain> oldSwapChain = std::move(m_vgeSwapChain);
        m_vgeSwapChain = std::make_unique<VgeSwapChain>(m_vgeDevice, extent, oldSwapChain);

        if (!oldSwapChain->compareSwapFormats(*m_vgeSwapChain.get())) {
            throw std::runtime_error("Swap chain image(or depth) format has changed!");
        }
    }
}

/* Creates command buffers for rendering.
 *
 * This function allocates command buffers for rendering operations,
 * resizing the command buffer array to accommodate the maximum number
 * of frames in flight. It throws an exception if the allocation fails.
 */
void VgeRenderer::createCommandBuffers()
{
    m_commandBuffers.resize(VgeSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_vgeDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

    if (vkAllocateCommandBuffers(m_vgeDevice.getDevice(), &allocInfo, m_commandBuffers.data()) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffersf!");
    }
}

/* Frees the allocated command buffers.
 *
 * This function releases the memory associated with the command buffers
 * and clears the buffer array.
 */
void VgeRenderer::freeCommandBuffers()
{
    vkFreeCommandBuffers(
        m_vgeDevice.getDevice(),
        m_vgeDevice.getCommandPool(),
        static_cast<uint32_t>(m_commandBuffers.size()),
        m_commandBuffers.data());

    m_commandBuffers.clear();
}

/* Begins the frame for rendering.
 *
 * This function acquires the next image from the swap chain and prepares
 * the command buffer for recording. It throws an exception if it fails
 * to acquire the image or begin the command buffer.
 */
VkCommandBuffer VgeRenderer::beginFrame()
{
    assert(!m_isFrameStarted && "Can't call beginFrame while already in progress!");

    VkResult result = m_vgeSwapChain->acquireNextImage(&m_currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return nullptr;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    m_isFrameStarted = true;

    VkCommandBuffer commandBuffer = getCurrentCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    return commandBuffer;
}

/* Ends the current rendering frame and submits the command buffer for
 * presentation.
 *
 * This method finalizes the recorded commands and handles any necessary
 * cleanup, including checking if the swap chain needs to be recreated.
 */
void VgeRenderer::endFrame()
{
    assert(m_isFrameStarted && "Can't call endFrame while frame not in progress!");

    VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
    VkResult result = m_vgeSwapChain->submitCommandBuffers(&commandBuffer, &m_currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        m_vgeWindow.wasWindowResized())
    {
        m_vgeWindow.resetWindowResizedFlag();
        recreateSwapChain();
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    m_isFrameStarted = false;
    m_currentFrameIndex = (m_currentFrameIndex + 1) % VgeSwapChain::MAX_FRAMES_IN_FLIGHT;
}

/* Begins the render pass for the current frame's swap chain.
 *
 * This method sets up the render pass with the appropriate framebuffer and
 * clear values, allowing for rendering operations to be recorded.
 */
void VgeRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    assert(m_isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress!");
    assert(
        commandBuffer == getCurrentCommandBuffer() &&
        "Can't begin render pass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_vgeSwapChain->getRenderPass();
    renderPassInfo.framebuffer = m_vgeSwapChain->getFrameBuffer(m_currentImageIndex);

    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_vgeSwapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f }; // index 0 is color
    clearValues[1].depthStencil = { 1.0f, 0 };            // index 1 is depth
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());

    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_vgeSwapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(m_vgeSwapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{
        { 0, 0 },
        m_vgeSwapChain->getSwapChainExtent()
    };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

/* Ends the render pass for the current frame's swap chain.
 *
 * This method finalizes the render pass and allows the command buffer to be
 * submitted.
 */
void VgeRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    assert(m_isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress!");
    assert(
        commandBuffer == getCurrentCommandBuffer() &&
        "Can't end render pass on command buffer from a different frame");
    vkCmdEndRenderPass(commandBuffer);
}

/* Retrieves the render pass used by the swap chain.
 *
 * This method returns the Vulkan render pass associated with the current swap
 * chain, allowing for further rendering operations.
 */
VkRenderPass VgeRenderer::getSwapChainRenderPass() const
{
    return m_vgeSwapChain->getRenderPass();
}

/* Calculates the aspect ratio of the swap chain.
 *
 * This method provides the aspect ratio based on the current swap chain extent,
 * useful for maintaining correct rendering proportions.
 */
float VgeRenderer::getAspectRatio() const
{
    return m_vgeSwapChain->extentAspectRatio();
}

/* Checks if a rendering frame is currently in progress.
 *
 * This method returns a boolean indicating whether the renderer is currently
 * in the process of rendering a frame.
 */
bool VgeRenderer::isFrameInProgress() const
{
    return m_isFrameStarted;
}

/* Retrieves the command buffer for the current rendering frame.
 *
 * This method returns the command buffer that is currently being recorded,
 * ensuring that commands are added to the correct buffer.
 */
VkCommandBuffer VgeRenderer::getCurrentCommandBuffer() const
{
    assert(m_isFrameStarted && "Cannot get command buffer when frame not in progress");
    return m_commandBuffers[m_currentFrameIndex];
}

/* Retrieves the index of the current rendering frame.
 *
 * This method returns the index of the frame being rendered, useful for
 * managing frame-related resources and logic.
 */
uint32_t VgeRenderer::getFrameIndex() const
{
    assert(m_isFrameStarted && "Cannot get frame index when frame not in progress");
    return m_currentFrameIndex;
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
        glm::vec2 ab = 0.5f * (a + b);
        glm::vec2 ac = 0.5f * (a + c);
        glm::vec2 bc = 0.5f * (b + c);
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

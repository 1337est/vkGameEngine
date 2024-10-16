#include "vge_app.hpp"
#include "max_frames_in_flight.hpp"
#include <exception>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vge {
VgeApp::VgeApp()
    : m_vgeWindow{ WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Vulkan!" }
    , m_vgeInstance{}
    , m_vgeSurface{ m_vgeInstance.getInstance(), m_vgeWindow.getWindow() }
    , m_vgeDevice{ m_vgeInstance.getInstance(),
                   m_vgeSurface.getSurface(),
                   m_vgeInstance.areVLayersEnabled(),
                   m_vgeInstance.getVLayers() }
    , m_vgeSwapchain{ m_vgeDevice.getPDevice(),        m_vgeSurface.getSurface(),
                      m_vgeDevice.getGraphicsFamily(), m_vgeDevice.getPresentFamily(),
                      m_vgeDevice.getLDevice(),        m_vgeWindow.getWindow() }
    , m_vgeImageView{ m_vgeDevice.getLDevice(),
                      m_vgeSwapchain.getSwapchainImages(),
                      m_vgeSwapchain.getSwapchainImageFormat() }
    , m_vgeRenderPass(m_vgeDevice.getLDevice(), m_vgeSwapchain.getSwapchainImageFormat())
    , m_vgePipeline{ m_vgeDevice.getLDevice(),
                     "build/shaders/shader.vert.spv",
                     "build/shaders/shader.frag.spv",
                     m_vgeRenderPass.getRenderPass() }
    , m_vgeFramebuffer{ m_vgeDevice.getLDevice(),
                        m_vgeImageView.getSwapchainImageViews(),
                        m_vgeSwapchain.getSwapchainExtent(),
                        m_vgeRenderPass.getRenderPass() }
    , m_vgeCommandPool{ m_vgeDevice.getLDevice(), m_vgeDevice.getGraphicsFamily() }
    , m_vgeCommandBuffer{ m_vgeDevice.getLDevice(), m_vgeCommandPool.getCommandPool() }
    , m_vgeSyncObjects{ m_vgeDevice.getLDevice() }

{
    try {
        mainLoop();
    }
    catch (const std::exception& e) {
        throw; // Rethrow the exception to be caught in main.cpp
    }
}

void VgeApp::mainLoop()
{
    // run until window closes
    while (!m_vgeWindow.shouldClose()) {
        glfwPollEvents(); // continuously processes and returns received events
        drawFrame();
    }
    vkDeviceWaitIdle(m_vgeDevice.getLDevice());
}

void VgeApp::drawFrame()
{
    // Wait for the fence to ensure that the previous frame has finished
    // rendering. This ensures that the GPU has completed processing of the last
    // submitted frame before proceeding with the next one.
    VkDevice lDevice = m_vgeDevice.getLDevice();
    std::vector<VkFence> inFlightFences = m_vgeSyncObjects.getInFlightFence();
    vkWaitForFences(lDevice, 1, &inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(lDevice, 1, &inFlightFences[m_currentFrame]);

    // Acquire the next image from the swapchain, waiting until it's available
    // using the imageAvailableSemaphore. The acquired image index will be used
    // for rendering.
    VkSwapchainKHR swapchain = m_vgeSwapchain.getSwapchain();
    std::vector<VkSemaphore> imageAvailableSemaphores =
        m_vgeSyncObjects.getImageAvailableSemaphore();
    uint32_t imageIndex;
    vkAcquireNextImageKHR(
        lDevice,
        swapchain,
        UINT64_MAX,
        imageAvailableSemaphores[m_currentFrame],
        VK_NULL_HANDLE,
        &imageIndex);

    // Reset the command buffer so it can be reused for recording commands for
    // the current frame. This avoids creating new command buffers each frame,
    // improving performance.
    std::vector<VkCommandBuffer> commandBuffers = m_vgeCommandBuffer.getCommandBuffers();
    vkResetCommandBuffer(
        commandBuffers[m_currentFrame],
        /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(commandBuffers[m_currentFrame], imageIndex);

    // Submit the recorded command buffer to the graphics queue.
    // The queue will wait for the image to become available (via
    // imageAvailableSemaphore), execute the commands in the command buffer, and
    // signal renderFinishedSemaphore upon completion.
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[m_currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    std::vector<VkSemaphore> renderFinishedSemaphores =
        m_vgeSyncObjects.getRenderFinishedSemaphore();
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[m_currentFrame] };
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffers[m_currentFrame],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores,
    };

    // Submit the command buffer to the graphics queue, using the inFlightFence
    // to track when the work is done. This fence will be used to synchronize
    // with the CPU and ensure the command buffer is not reused prematurely.
    VkQueue graphicsQueue = m_vgeDevice.getGraphicsQueue();
    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[m_currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // Present the rendered image to the swapchain. The present operation will
    // wait for the renderFinishedSemaphore to ensure rendering is complete
    // before displaying the image.
    VkSwapchainKHR swapChains[] = { swapchain };
    VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapChains,
        .pImageIndices = &imageIndex,
        .pResults = nullptr,
    };

    // Submit the present request to the presentation queue, displaying the
    // rendered image to the screen.
    VkQueue presentQueue = m_vgeDevice.getPresentQueue();
    vkQueuePresentKHR(presentQueue, &presentInfo);

    // iterate currentFrame
    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VgeApp::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    // Begin recording the command buffer, preparing it to receive draw
    // commands. This structure defines the parameters for command buffer
    // recording.
    VkCommandBufferBeginInfo commandBufferBeginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,                  // Optional
        .pInheritanceInfo = nullptr, // Optional
    };
    if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    // Begin a new render pass. This sets up the framebuffer, render area, and
    // clear color. The render pass defines the structure of the rendering
    // process and how different operations like color blending or depth testing
    // happen.
    VkRenderPass renderPass = m_vgeRenderPass.getRenderPass();
    std::vector<VkFramebuffer> framebuffers = m_vgeFramebuffer.getFramebuffers();
    VkExtent2D swapchainExtent = m_vgeSwapchain.getSwapchainExtent();
    VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
    VkRenderPassBeginInfo renderPassBeginInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = renderPass,
        .framebuffer = framebuffers[imageIndex], // Use the framebuffer associated with the current swapchain image.
        .renderArea = {
            .offset = { 0, 0 },                 // The render area starts at the top-left corner of the framebuffer.
            .extent =  swapchainExtent,         // The size of the render area matches the swapchain extent.
        },
        .clearValueCount = 1,                   // Number of clear values (color, depth, etc.).
        .pClearValues = &clearColor,            // Clear color value to use.
    };
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Bind the graphics pipeline, which contains the shader programs and
    // fixed-function state for rendering. This pipeline will be used for the
    // subsequent draw commands.
    VkPipeline graphicsPipeline = m_vgePipeline.getGraphicsPipeline();
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    // Define the viewport for rendering, specifying the size of the area on the
    // screen to which the output will be mapped.
    VkViewport viewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(swapchainExtent.width),
        .height = static_cast<float>(swapchainExtent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // Define the scissor rectangle, which restricts rendering to a specific
    // part of the framebuffer. Here, it matches the full size of the swapchain
    // extent.
    VkRect2D scissor{
        .offset = { 0, 0 },
        .extent = swapchainExtent,
    };
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // Issue a draw command, instructing Vulkan to render 3 vertices. This
    // typically draws a single triangle. This is the core of the rendering
    // process.
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    // End the render pass, signaling that rendering commands for this pass are
    // complete.
    vkCmdEndRenderPass(commandBuffer);

    // Complete recording of the command buffer.
    // Once ended, the command buffer can be submitted to the graphics queue for
    // execution.
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

} // namespace vge

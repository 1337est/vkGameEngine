#include "vge_app.hpp"
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
    , m_vgeSwapchain{ m_vgeDevice.getPDevice(),       m_vgeSurface.getSurface(), m_vgeDevice.getGraphicsFamily(),
                      m_vgeDevice.getPresentFamily(), m_vgeDevice.getLDevice(),  m_vgeWindow.getWindow() }
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
    , m_vgeCommandPool{ m_vgeDevice.getLDevice(),
                        m_vgeDevice.getGraphicsFamily(),
                        m_vgeRenderPass.getRenderPass(),
                        m_vgeFramebuffer.getFramebuffers(),
                        m_vgeSwapchain.getSwapchainExtent(),
                        m_vgePipeline.getGraphicsPipeline() }
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
    // Wait for the fence to ensure that the previous frame has finished rendering
    VkDevice lDevice = m_vgeDevice.getLDevice();
    VkFence inFlightFence = m_vgeSyncObjects.getInFlightFence();
    vkWaitForFences(lDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(lDevice, 1, &inFlightFence);

    // Acquire the next image from the swapchain, waiting on the imageAvailableSemaphore
    VkSwapchainKHR swapchain = m_vgeSwapchain.getSwapchain();
    VkSemaphore imageAvailableSemaphore = m_vgeSyncObjects.getImageAvailableSemaphore();
    uint32_t imageIndex;
    vkAcquireNextImageKHR(lDevice, swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    // Reset the command buffer and record commands to it for the current image
    VkCommandBuffer commandBuffer = m_vgeCommandPool.getCommandBuffer();
    vkResetCommandBuffer(commandBuffer, /*VkCommandBufferResetFlagBits*/ 0);
    m_vgeCommandPool.recordCommandBuffer(imageIndex);

    // Submit the recorded command buffer to the graphics queue, ensuring it waits for the image to be available
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore renderFinishedSemaphore = m_vgeSyncObjects.getRenderFinishedSemaphore();
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores,
    };

    // Submit the command buffer to the graphics queue and use a fence to track its completion
    VkQueue graphicsQueue = m_vgeDevice.getGraphicsQueue();
    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // Present the rendered image to the swapchain, waiting for the renderFinishedSemaphore
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

    // Submit the present request to the presentation queue
    VkQueue presentQueue = m_vgeDevice.getPresentQueue();
    vkQueuePresentKHR(presentQueue, &presentInfo);
}

} // namespace vge

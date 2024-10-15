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
    VkDevice lDevice = m_vgeDevice.getLDevice();
    VkFence inFlightFence = m_vgeSyncObjects.getInFlightFence();
    vkWaitForFences(lDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(lDevice, 1, &inFlightFence);

    VkSwapchainKHR swapchain = m_vgeSwapchain.getSwapchain();
    VkSemaphore imageAvailableSemaphore = m_vgeSyncObjects.getImageAvailableSemaphore();
    uint32_t imageIndex;
    vkAcquireNextImageKHR(lDevice, swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    VkCommandBuffer commandBuffer = m_vgeCommandPool.getCommandBuffer();
    vkResetCommandBuffer(commandBuffer, /*VkCommandBufferResetFlagBits*/ 0);
    m_vgeCommandPool.recordCommandBuffer(imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore renderFinishedSemaphore = m_vgeSyncObjects.getRenderFinishedSemaphore();
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkQueue graphicsQueue = m_vgeDevice.getGraphicsQueue();
    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    VkQueue presentQueue = m_vgeDevice.getPresentQueue();
    vkQueuePresentKHR(presentQueue, &presentInfo);
}

} // namespace vge

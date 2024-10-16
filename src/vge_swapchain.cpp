#include "vge_swapchain.hpp"

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>

namespace vge {

/* VgeSwapChain constructor
 *
 * Initializes a new instance of the VgeSwapChain class with the specified
 * Vulkan device and swap chain extent. This constructor sets up the swap chain
 * and its related resources.
 */
VgeSwapChain::VgeSwapChain(VgeDevice& deviceRef, VkExtent2D extent)
    : m_swapChainImageFormat{}
    , m_swapChainDepthFormat{}
    , m_swapChainExtent{}
    , m_swapChainFramebuffers{}
    , m_renderPass{}
    , m_depthImages{}
    , m_depthImageMemorys{}
    , m_depthImageViews{}
    , m_swapChainImages{}
    , m_swapChainImageViews{}
    , m_device{ deviceRef }
    , m_windowExtent{ extent }
    , m_swapChain{}
    , m_oldSwapChain{}
    , m_imageAvailableSemaphores{}
    , m_renderFinishedSemaphores{}
    , m_inFlightFences{}
    , m_imagesInFlight{}
{
    initSwapChain();
}

/* VgeSwapChain constructor with previous swap chain
 *
 * Initializes a new instance of the VgeSwapChain class with the specified
 * Vulkan device, swap chain extent, and a shared pointer to the previous swap
 * chain. Cleans up the old swap chain after initializing the new one.
 */
VgeSwapChain::VgeSwapChain(
    VgeDevice& deviceRef,
    VkExtent2D extent,
    std::shared_ptr<VgeSwapChain> previous)
    : m_swapChainImageFormat{}
    , m_swapChainDepthFormat{}
    , m_swapChainExtent{}
    , m_swapChainFramebuffers{}
    , m_renderPass{}
    , m_depthImages{}
    , m_depthImageMemorys{}
    , m_depthImageViews{}
    , m_swapChainImages{}
    , m_swapChainImageViews{}
    , m_device{ deviceRef }
    , m_windowExtent{ extent }
    , m_swapChain{}
    , m_oldSwapChain{ previous }
    , m_imageAvailableSemaphores{}
    , m_renderFinishedSemaphores{}
    , m_inFlightFences{}
    , m_imagesInFlight{}
{
    initSwapChain();

    // clean up old swap chain since it's no longer needed
    m_oldSwapChain = nullptr;
}

/* Initializes the swap chain and its related resources
 *
 * This function creates the swap chain, image views, render pass, depth
 * resources, framebuffers, and synchronization objects necessary for rendering.
 */
void VgeSwapChain::initSwapChain()
{
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDepthResources();
    createFramebuffers();
    createSyncObjects();
}

/* VgeSwapChain destructor
 *
 * Cleans up and releases all resources associated with the swap chain,
 * including image views, framebuffers, render pass, depth resources, and
 * synchronization objects.
 */
VgeSwapChain::~VgeSwapChain()
{
    for (VkImageView_T* imageView : m_swapChainImageViews) {
        vkDestroyImageView(m_device.getDevice(), imageView, nullptr);
    }
    m_swapChainImageViews.clear();

    if (m_swapChain != nullptr) {
        vkDestroySwapchainKHR(m_device.getDevice(), m_swapChain, nullptr);
        m_swapChain = nullptr;
    }

    for (int i = 0; i < static_cast<int>(m_depthImages.size()); i++) {
        vkDestroyImageView(
            m_device.getDevice(),
            m_depthImageViews[static_cast<size_t>(i)],
            nullptr);
        vkDestroyImage(m_device.getDevice(), m_depthImages[static_cast<size_t>(i)], nullptr);
        vkFreeMemory(m_device.getDevice(), m_depthImageMemorys[static_cast<size_t>(i)], nullptr);
    }

    for (VkFramebuffer_T* framebuffer : m_swapChainFramebuffers) {
        vkDestroyFramebuffer(m_device.getDevice(), framebuffer, nullptr);
    }

    vkDestroyRenderPass(m_device.getDevice(), m_renderPass, nullptr);

    // cleanup synchronization objects
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(m_device.getDevice(), m_renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(m_device.getDevice(), m_imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(m_device.getDevice(), m_inFlightFences[i], nullptr);
    }
}

/* Acquires the next image in the swap chain
 *
 * Waits for the specified fence and acquires the next image from the swap
 * chain, storing the index of the acquired image in the provided imageIndex
 * pointer.
 *
 * @param imageIndex Pointer to a variable that will receive the index of the
 * acquired image.
 * @return Result of the image acquisition operation.
 */
VkResult VgeSwapChain::acquireNextImage(uint32_t* imageIndex)
{
    vkWaitForFences(
        m_device.getDevice(),
        1,
        &m_inFlightFences[m_currentFrame],
        VK_TRUE,
        std::numeric_limits<uint64_t>::max());

    VkResult result = vkAcquireNextImageKHR(
        m_device.getDevice(),
        m_swapChain,
        std::numeric_limits<uint64_t>::max(),
        m_imageAvailableSemaphores[m_currentFrame], // must be a not signaled
                                                    // semaphore
        VK_NULL_HANDLE,
        imageIndex);

    return result;
}

/* Submits command buffers for execution
 *
 * Submits the specified command buffers to the graphics queue and presents the
 * acquired image. Waits for the previous frame's in-flight fence if necessary.
 *
 * @param buffers Pointer to the command buffers to submit.
 * @param imageIndex Pointer to the index of the acquired image to present.
 * @return Result of the submission and presentation operation.
 */
VkResult VgeSwapChain::submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex)
{
    if (m_imagesInFlight[*imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(
            m_device.getDevice(),
            1,
            &m_imagesInFlight[*imageIndex],
            VK_TRUE,
            UINT64_MAX);
    }
    m_imagesInFlight[*imageIndex] = m_inFlightFences[m_currentFrame];

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = buffers;

    VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(m_device.getDevice(), 1, &m_inFlightFences[m_currentFrame]);
    if (vkQueueSubmit(
            m_device.getGraphicsQueue(),
            1,
            &submitInfo,
            m_inFlightFences[m_currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { m_swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = imageIndex;

    VkResult result = vkQueuePresentKHR(m_device.getPresentQueue(), &presentInfo);

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}

/* Creates the swap chain
 *
 * Configures and creates the swap chain based on the supported formats, present
 * modes, and capabilities of the Vulkan device.
 */
void VgeSwapChain::createSwapChain()
{
    SwapChainSupportDetails swapChainSupport = m_device.getSwapChainSupport();

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_device.getSurface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = m_device.findPhysicalQueueFamilies();
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain =
        m_oldSwapChain == nullptr ? VK_NULL_HANDLE : m_oldSwapChain->m_swapChain;

    if (vkCreateSwapchainKHR(m_device.getDevice(), &createInfo, nullptr, &m_swapChain) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    // we only specified a minimum number of images in the swap chain, so the
    // implementation is allowed to create a swap chain with more. That's why
    // we'll first query the final number of images with
    // vkGetSwapchainImagesKHR, then resize the container and finally call it
    // again to retrieve the handles.
    vkGetSwapchainImagesKHR(m_device.getDevice(), m_swapChain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(
        m_device.getDevice(),
        m_swapChain,
        &imageCount,
        m_swapChainImages.data());

    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
}

/* Creates image views for the swap chain images
 *
 * Iterates through the swap chain images and creates a corresponding image view
 * for each one.
 */
void VgeSwapChain::createImageViews()
{
    m_swapChainImageViews.resize(m_swapChainImages.size());
    for (size_t i = 0; i < m_swapChainImages.size(); i++) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_swapChainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = m_swapChainImageFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(
                m_device.getDevice(),
                &viewInfo,
                nullptr,
                &m_swapChainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}

/* Creates the render pass for the swap chain
 *
 * Configures and creates the render pass that defines how rendering will be
 * done to the swap chain images and depth attachments.
 */
void VgeSwapChain::createRenderPass()
{
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = getSwapChainImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstSubpass = 0;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(m_device.getDevice(), &renderPassInfo, nullptr, &m_renderPass) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

/* Creates framebuffers for the swap chain
 *
 * Creates framebuffers that connect the swap chain image views and depth image
 * views to the render pass for rendering operations.
 */
void VgeSwapChain::createFramebuffers()
{
    m_swapChainFramebuffers.resize(imageCount());
    for (size_t i = 0; i < imageCount(); i++) {
        std::array<VkImageView, 2> attachments = { m_swapChainImageViews[i], m_depthImageViews[i] };

        VkExtent2D swapChainExtent = getSwapChainExtent();
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(
                m_device.getDevice(),
                &framebufferInfo,
                nullptr,
                &m_swapChainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

/* Creates depth resources for the swap chain
 *
 * Allocates and creates depth images and views necessary for rendering with
 * depth attachments in the swap chain.
 */
void VgeSwapChain::createDepthResources()
{
    VkFormat depthFormat = findDepthFormat();
    m_swapChainDepthFormat = depthFormat;
    VkExtent2D swapChainExtent = getSwapChainExtent();

    m_depthImages.resize(imageCount());
    m_depthImageMemorys.resize(imageCount());
    m_depthImageViews.resize(imageCount());

    for (int i = 0; i < static_cast<int>(m_depthImages.size()); i++) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = swapChainExtent.width;
        imageInfo.extent.height = swapChainExtent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = depthFormat;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = 0;

        m_device.createImageWithInfo(
            imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_depthImages[static_cast<size_t>(i)],
            m_depthImageMemorys[static_cast<size_t>(i)]);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_depthImages[static_cast<size_t>(i)];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = depthFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(
                m_device.getDevice(),
                &viewInfo,
                nullptr,
                &m_depthImageViews[static_cast<size_t>(i)]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}

/* Creates synchronization objects for managing the rendering process in Vulkan.
 *
 * This function initializes semaphores and fences to synchronize image
 * presentation and rendering operations for multiple frames in flight.
 */
void VgeSwapChain::createSyncObjects()
{
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    m_imagesInFlight.resize(imageCount(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(
                m_device.getDevice(),
                &semaphoreInfo,
                nullptr,
                &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(
                m_device.getDevice(),
                &semaphoreInfo,
                nullptr,
                &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_device.getDevice(), &fenceInfo, nullptr, &m_inFlightFences[i]) !=
                VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

/* Chooses the best surface format for the swap chain.
 *
 * This function iterates through available surface formats and returns the
 * format that supports the VK_FORMAT_B8G8R8A8_UNORM format and the
 * VK_COLOR_SPACE_SRGB_NONLINEAR_KHR color space.
 */
VkSurfaceFormatKHR VgeSwapChain::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const VkSurfaceFormatKHR& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

/* Chooses the best present mode for the swap chain.
 *
 * This function checks available present modes and selects the mailbox mode if
 * available, or defaults to V-Sync mode if not.
 */
VkPresentModeKHR VgeSwapChain::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const VkPresentModeKHR& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            std::cout << "Present mode: Mailbox" << std::endl;
            return availablePresentMode;
        }
    }

    // for (const VkPresentModeKHR& availablePresentMode :
    // availablePresentModes) {
    //   if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
    //     std::cout << "Present mode: Immediate" << std::endl;
    //     return availablePresentMode;
    //   }
    // }

    std::cout << "Present mode: V-Sync" << std::endl;
    return VK_PRESENT_MODE_FIFO_KHR;
}

/* Chooses the swap extent for the swap chain.
 *
 * This function determines the dimensions of the swap chain images based on
 * the surface capabilities and the desired window size.
 */
VkExtent2D VgeSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        VkExtent2D actualExtent = m_windowExtent;
        actualExtent.width = std::max(
            capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

/* Finds a suitable depth format for the swap chain.
 *
 * This function queries the device for supported depth formats suitable for
 * depth and stencil attachments.
 */
VkFormat VgeSwapChain::findDepthFormat()
{
    return m_device.findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

/* Retrieves the framebuffer at the specified index.
 *
 * This function returns the framebuffer object associated with the given index
 * in the swap chain.
 */
VkFramebuffer VgeSwapChain::getFrameBuffer(size_t index)
{
    return m_swapChainFramebuffers[index];
}

/* Retrieves the render pass used by the swap chain.
 *
 * This function returns the Vulkan render pass object configured for rendering
 * with the swap chain.
 */
VkRenderPass VgeSwapChain::getRenderPass()
{
    return m_renderPass;
}

/* Retrieves the image view at the specified index.
 *
 * This function returns the image view object associated with the given index
 * in the swap chain.
 */
VkImageView VgeSwapChain::getImageView(size_t index)
{
    return m_swapChainImageViews[index];
}

/* Returns the number of images in the swap chain.
 *
 * This function returns the count of images currently managed by the swap
 * chain.
 */
size_t VgeSwapChain::imageCount()
{
    return m_swapChainImages.size();
}

/* Retrieves the format of the swap chain images.
 *
 * This function returns the Vulkan format used for the swap chain images.
 */
VkFormat VgeSwapChain::getSwapChainImageFormat()
{
    return m_swapChainImageFormat;
}

/* Retrieves the extent of the swap chain.
 *
 * This function returns the dimensions of the swap chain images.
 */
VkExtent2D VgeSwapChain::getSwapChainExtent()
{
    return m_swapChainExtent;
}

/* Retrieves the width of the swap chain extent.
 *
 * This function returns the width of the swap chain's image extent.
 */
uint32_t VgeSwapChain::width()
{
    return m_swapChainExtent.width;
}

/* Retrieves the height of the swap chain extent.
 *
 * This function returns the height of the swap chain's image extent.
 */
uint32_t VgeSwapChain::height()
{
    return m_swapChainExtent.height;
}

/* Calculates the aspect ratio of the swap chain extent.
 *
 * This function computes and returns the aspect ratio of the swap chain's image
 * extent (width divided by height).
 */
float VgeSwapChain::extentAspectRatio()
{
    return static_cast<float>(m_swapChainExtent.width) /
           static_cast<float>(m_swapChainExtent.height);
}

/* Compares the swap formats of two swap chain objects.
 *
 * This function checks if the depth format and image format of the current swap
 * chain match those of the provided swap chain.
 */
bool VgeSwapChain::compareSwapFormats(const VgeSwapChain& swapChain) const
{
    return swapChain.m_swapChainDepthFormat == m_swapChainDepthFormat &&
           swapChain.m_swapChainImageFormat == m_swapChainImageFormat;
}
} // namespace vge

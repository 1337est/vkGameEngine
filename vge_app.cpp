// headers
#include "vge_app.hpp"
#include "vge_buffer.hpp"
#include "vge_camera.hpp"
#include "vge_descriptors.hpp"
#include "vge_keyboard_movement_controller.hpp"
#include "vge_render_system.hpp"
#include <vulkan/vulkan_core.h>

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>

namespace vge
{

struct GlobalUbo // Uniform buffer object
{
    alignas(16) glm::mat4 projectionView{ 1.f };
    alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f,
                                                                     -3.f,
                                                                     -1.f });
};

VgeApp::VgeApp()
    : m_gameObjects{}
{
    m_globalPool = VgeDescriptorPool::Builder(m_vgeDevice)
                       .setMaxSets(VgeSwapChain::MAX_FRAMES_IN_FLIGHT)
                       .addPoolSize(
                           VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                           VgeSwapChain::MAX_FRAMES_IN_FLIGHT)
                       .build();
    loadGameObjects();
}

VgeApp::~VgeApp()
{
}

void VgeApp::run()
{
    std::vector<std::unique_ptr<VgeBuffer>> uboBuffers(
        VgeSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++)
    {
        uboBuffers[i] = std::make_unique<VgeBuffer>(
            m_vgeDevice,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->map();
    }

    auto globalSetLayout = VgeDescriptorSetLayout::Builder(m_vgeDevice)
                               .addBinding(
                                   0,
                                   VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                   VK_SHADER_STAGE_VERTEX_BIT)
                               .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(
        VgeSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++)
    {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        VgeDescriptorWriter(*globalSetLayout, *m_globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
    }

    VgeRenderSystem renderSystem{
        m_vgeDevice,
        m_vgeRenderer.getSwapChainRenderPass(),
        globalSetLayout->getDescriptorSetLayout(),
    };

    VgeCamera camera{};
    camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    auto viewerObject = VgeGameObject::createGameObject();
    VgaKeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

    // run until window closes
    while (!m_vgeWindow.shouldClose())
    {
        glfwPollEvents(); // continuously processes and returns received events

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime =
            std::chrono::duration<float, std::chrono::seconds::period>(
                newTime - currentTime)
                .count();
        currentTime = newTime;

        cameraController.moveInPlaneXZ(
            m_vgeWindow.getGLFWwindow(),
            frameTime,
            viewerObject);
        camera.setViewYXZ(
            viewerObject.m_transform.translation,
            viewerObject.m_transform.rotation);

        float aspect = m_vgeRenderer.getAspectRatio();
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

        // beginFrame returns nullptr if swapchain needs to be recreated
        if (auto commandBuffer = m_vgeRenderer.beginFrame())
        {
            int frameIndex = m_vgeRenderer.getFrameIndex();
            FrameInfo frameInfo{
                frameIndex,
                frameTime,
                commandBuffer,
                camera,
                globalDescriptorSets[frameIndex],
            };

            // update
            GlobalUbo ubo{};
            ubo.projectionView = camera.getProjection() * camera.getView();
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();

            // render
            m_vgeRenderer.beginSwapChainRenderPass(commandBuffer);
            renderSystem.renderGameObjects(frameInfo, m_gameObjects);
            m_vgeRenderer.endSwapChainRenderPass(commandBuffer);
            m_vgeRenderer.endFrame();
        }
    }

    vkDeviceWaitIdle(m_vgeDevice.device());
}

void VgeApp::loadGameObjects()
{
    std::shared_ptr<VgeModel> vgeModel =
        VgeModel::createModelFromFile(m_vgeDevice, "models/flat_vase.obj");
    auto flatVase = VgeGameObject::createGameObject();
    flatVase.m_model = vgeModel;
    flatVase.m_transform.translation = { -.5f, .5f, 2.5f };
    flatVase.m_transform.scale = { 3.f, 1.5f, 3.f };
    m_gameObjects.push_back(std::move(flatVase));

    vgeModel =
        VgeModel::createModelFromFile(m_vgeDevice, "models/smooth_vase.obj");
    auto smoothVase = VgeGameObject::createGameObject();
    smoothVase.m_model = vgeModel;
    smoothVase.m_transform.translation = { .5f, .5f, 2.5f };
    smoothVase.m_transform.scale = { 3.f, 1.5f, 3.f };
    m_gameObjects.push_back(std::move(smoothVase));
}

} // namespace vge

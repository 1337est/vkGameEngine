#include "vge_app.hpp"
#include "systems/vge_point_light_system.hpp"
#include "systems/vge_render_system.hpp"
#include "vge_buffer.hpp"
#include "vge_camera.hpp"
#include "vge_descriptors.hpp"
#include "vge_keyboard_movement_controller.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <chrono>

namespace vge {
/* Constructs a VgeApp object.
 *
 * Initializes the application by setting up the global descriptor pool
 * and loading game objects into the scene.
 */
VgeApp::VgeApp()
    : m_vgeWindow{ WIDTH, HEIGHT, "Hello Vulkan!" }
    , m_vgeDevice{ m_vgeWindow }
    , m_vgeRenderer{ m_vgeWindow, m_vgeDevice }
    , m_globalPool{}
    , m_gameObjects{}
{
    m_globalPool =
        VgeDescriptorPool::Builder(m_vgeDevice)
            .setMaxSets(VgeSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VgeSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
    loadGameObjects();
}

/* Destroys the VgeApp object.
 *
 * Cleans up resources associated with the VgeApp instance.
 */
VgeApp::~VgeApp()
{}

/* Runs the main loop of the application.
 *
 * Handles input events, updates the camera and game objects, and
 * manages the rendering process for each frame until the window is closed.
 */
void VgeApp::run()
{
    std::vector<std::unique_ptr<VgeBuffer>> uboBuffers(VgeSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] = std::make_unique<VgeBuffer>(
            m_vgeDevice,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->map();
    }

    std::unique_ptr<VgeDescriptorSetLayout> globalSetLayout =
        VgeDescriptorSetLayout::Builder(m_vgeDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(VgeSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < globalDescriptorSets.size(); i++) {
        VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->descriptorInfo();
        VgeDescriptorWriter(*globalSetLayout, *m_globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
    }

    VgeRenderSystem renderSystem{
        m_vgeDevice,
        m_vgeRenderer.getSwapChainRenderPass(),
        globalSetLayout->getDescriptorSetLayout(),
    };
    VgePointLightSystem pointLightSystem{
        m_vgeDevice,
        m_vgeRenderer.getSwapChainRenderPass(),
        globalSetLayout->getDescriptorSetLayout(),
    };

    VgeCamera camera{};
    camera.setViewTargetDirectionMatrix(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    VgeGameObject viewerObject = VgeGameObject::createGameObject();
    viewerObject.m_transform.translation.z = -2.5f;
    VgeKeyboardMovementController cameraController{};

    std::chrono::time_point currentTime = std::chrono::high_resolution_clock::now();

    // run until window closes
    while (!m_vgeWindow.shouldClose()) {
        glfwPollEvents(); // continuously processes and returns received events

        std::chrono::time_point newTime = std::chrono::high_resolution_clock::now();
        float frameTime =
            std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime)
                .count();
        currentTime = newTime;

        cameraController.moveInPlaneXZ(m_vgeWindow.getGLFWwindow(), frameTime, viewerObject);
        camera.setViewYXZMatrix(
            viewerObject.m_transform.translation,
            viewerObject.m_transform.rotation);

        float aspect = m_vgeRenderer.getAspectRatio();
        camera.setPerspectiveProjectionMatrix(glm::radians(50.f), aspect, 0.1f, 100.f);

        // beginFrame returns nullptr if swapchain needs to be recreated
        if (VkCommandBuffer commandBuffer = m_vgeRenderer.beginFrame()) {
            int frameIndex = m_vgeRenderer.getFrameIndex();
            FrameInfo frameInfo{
                frameIndex,    frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex],
                m_gameObjects,
            };

            // update
            GlobalUbo ubo{};
            ubo.projection = camera.getProjectionMatrix();
            ubo.view = camera.getViewMatrix();
            ubo.inverseView = camera.getInverseViewMatrix();
            pointLightSystem.update(frameInfo, ubo);
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();

            // render
            m_vgeRenderer.beginSwapChainRenderPass(commandBuffer);
            renderSystem.renderGameObjects(frameInfo);
            pointLightSystem.render(frameInfo);
            m_vgeRenderer.endSwapChainRenderPass(commandBuffer);
            m_vgeRenderer.endFrame();
        }
    }

    vkDeviceWaitIdle(m_vgeDevice.getDevice());
}

/* Loads game objects into the application.
 *
 * Creates models from files, sets their transformations, and stores
 * them in the game object container for rendering.
 */
void VgeApp::loadGameObjects()
{
    std::shared_ptr<VgeModel> vgeModel =
        VgeModel::createModelFromFile(m_vgeDevice, "models/flat_vase.obj");
    VgeGameObject flatVase = VgeGameObject::createGameObject();
    flatVase.m_model = vgeModel;
    flatVase.m_transform.translation = { -.5f, .5f, 0.f };
    flatVase.m_transform.scale = { 3.f, 1.5f, 3.f };
    m_gameObjects.emplace(flatVase.getId(), std::move(flatVase));

    vgeModel = VgeModel::createModelFromFile(m_vgeDevice, "models/smooth_vase.obj");
    VgeGameObject smoothVase = VgeGameObject::createGameObject();
    smoothVase.m_model = vgeModel;
    smoothVase.m_transform.translation = { .5f, .5f, 0.f };
    smoothVase.m_transform.scale = { 3.f, 1.5f, 3.f };
    m_gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

    vgeModel = VgeModel::createModelFromFile(m_vgeDevice, "models/quad.obj");
    VgeGameObject floor = VgeGameObject::createGameObject();
    floor.m_model = vgeModel;
    floor.m_transform.translation = { 0.f, .5f, 0.f };
    floor.m_transform.scale = { 3.f, 1.f, 3.f };
    m_gameObjects.emplace(floor.getId(), std::move(floor));

    std::vector<glm::vec3> lightColors{
        { 1.f, .1f, .1f },
        { .1f, .1f, 1.f },
        { .1f, 1.f, .1f },
        { 1.f, 1.f, .1f },
        { .1f, 1.f, 1.f },
        { 1.f, 1.f, 1.f }  //
    };

    for (size_t i = 0; i < lightColors.size(); i++) {
        VgeGameObject pointLight = VgeGameObject::makePointLight(0.2f);
        pointLight.m_color = lightColors[i];
        glm::mat<4, 4, float, (glm::qualifier)0U> rotateLight = glm::rotate(
            glm::mat4(1.f),
            (i * glm::two_pi<float>()) / lightColors.size(),
            { 0.f, -1.f, 0.f });
        pointLight.m_transform.translation =
            glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
        m_gameObjects.emplace(pointLight.getId(), std::move(pointLight));
    }
}

} // namespace vge

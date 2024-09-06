// headers
#include "vge_app.hpp"
#include "vge_camera.hpp"
#include "vge_keyboard_movement_controller.hpp"
#include "vge_render_system.hpp"

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

VgeApp::VgeApp()
    : m_gameObjects{}
{
    loadGameObjects();
}

VgeApp::~VgeApp()
{
}

void VgeApp::run()
{
    VgeRenderSystem renderSystem{ m_vgeDevice,
                                  m_vgeRenderer.getSwapChainRenderPass() };
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
            m_vgeRenderer.beginSwapChainRenderPass(commandBuffer);
            renderSystem.renderGameObjects(
                commandBuffer,
                m_gameObjects,
                camera);
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

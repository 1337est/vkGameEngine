// headers
#include "vge_app.hpp"
#include "vge_camera.hpp"
#include "vge_keyboard_movement_controller.hpp"
#include "vge_render_system.hpp"

// libraries
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

// temporary helper function, creates a 1x1x1 cube centered at offset
std::unique_ptr<VgeModel> createCubeModel(VgeDevice& device, glm::vec3 offset)
{
    VgeModel::Builder modelBuilder{};
    modelBuilder.vertices = {

        // left face (white)
        { { -.5f, -.5f, -.5f },  { .9f, .9f, .9f } },
        { { -.5f, .5f, .5f },    { .9f, .9f, .9f } },
        { { -.5f, -.5f, .5f },   { .9f, .9f, .9f } },
        { { -.5f, .5f, -.5f },   { .9f, .9f, .9f } },

        // right face (yellow)
        { { .5f, -.5f, -.5f },   { .8f, .8f, .1f } },
        { { .5f, .5f, .5f },     { .8f, .8f, .1f } },
        { { .5f, -.5f, .5f },    { .8f, .8f, .1f } },
        { { .5f, .5f, -.5f },    { .8f, .8f, .1f } },

        // top face (orange, remember y axis points down)
        { { -.5f, -.5f, -.5f },  { .9f, .6f, .1f } },
        { { .5f, -.5f, .5f },    { .9f, .6f, .1f } },
        { { -.5f, -.5f, .5f },   { .9f, .6f, .1f } },
        { { .5f, -.5f, -.5f },   { .9f, .6f, .1f } },

        // bottom face (red)
        { { -.5f, .5f, -.5f },   { .8f, .1f, .1f } },
        { { .5f, .5f, .5f },     { .8f, .1f, .1f } },
        { { -.5f, .5f, .5f },    { .8f, .1f, .1f } },
        { { .5f, .5f, -.5f },    { .8f, .1f, .1f } },

        // nose face (blue)
        { { -.5f, -.5f, 0.5f },  { .1f, .1f, .8f } },
        { { .5f, .5f, 0.5f },    { .1f, .1f, .8f } },
        { { -.5f, .5f, 0.5f },   { .1f, .1f, .8f } },
        { { .5f, -.5f, 0.5f },   { .1f, .1f, .8f } },

        // tail face (green)
        { { -.5f, -.5f, -0.5f }, { .1f, .8f, .1f } },
        { { .5f, .5f, -0.5f },   { .1f, .8f, .1f } },
        { { -.5f, .5f, -0.5f },  { .1f, .8f, .1f } },
        { { .5f, -.5f, -0.5f },  { .1f, .8f, .1f } },
    };

    for (auto& v : modelBuilder.vertices)
    {
        v.position += offset;
    }

    modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,
                             8,  9,  10, 8,  11, 9,  12, 13, 14, 12, 15, 13,
                             16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

    return std::make_unique<VgeModel>(device, modelBuilder);
}

void VgeApp::loadGameObjects()
{
    std::shared_ptr<VgeModel> vgeModel =
        createCubeModel(m_vgeDevice, { .0f, .0f, .0f });

    auto cube = VgeGameObject::createGameObject();
    cube.m_model = vgeModel;
    cube.m_transform.translation = { .0f, .0f, 2.5f };
    cube.m_transform.scale = { .5f, .5f, .5f };
    m_gameObjects.push_back(std::move(cube));
}

} // namespace vge

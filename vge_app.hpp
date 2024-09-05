#pragma once

// headers
#include "vge_device.hpp"
#include "vge_game_object.hpp"
#include "vge_renderer.hpp"
#include "vge_window.hpp"

// libraries
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

// std
#include <memory>
#include <vector>

namespace vge
{

class VgeApp
{
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    VgeApp();
    ~VgeApp();

    VgeApp(const VgeApp&) = delete;
    VgeApp& operator=(const VgeApp&) = delete;

    void run();

private:
    void loadGameObjects();

    VgeWindow m_vgeWindow{ WIDTH, HEIGHT, "Hello Vulkan!" }; //  window
    VgeDevice m_vgeDevice{ m_vgeWindow }; // use device for window
    VgeRenderer m_vgeRenderer{ m_vgeWindow, m_vgeDevice };
    std::vector<VgeGameObject> m_gameObjects;
};

} // namespace vge

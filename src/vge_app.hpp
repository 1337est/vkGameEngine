#pragma once

#include "vge_descriptors.hpp"
#include "vge_device.hpp"
#include "vge_game_object.hpp"
#include "vge_renderer.hpp"
#include "vge_window.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include <memory>

namespace vge {

class VgeApp {
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
    VgeDevice m_vgeDevice{ m_vgeWindow };                    // use device for window
    VgeRenderer m_vgeRenderer{ m_vgeWindow, m_vgeDevice };

    // note: order of declarations matters
    std::unique_ptr<VgeDescriptorPool> m_globalPool{};
    VgeGameObject::Map m_gameObjects;
};

} // namespace vge

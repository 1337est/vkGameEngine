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

    VgeWindow m_vgeWindow;
    VgeDevice m_vgeDevice;
    VgeRenderer m_vgeRenderer;

    // note: order of declarations matters
    std::unique_ptr<VgeDescriptorPool> m_globalPool;
    VgeGameObject::Map m_gameObjects;
};

} // namespace vge

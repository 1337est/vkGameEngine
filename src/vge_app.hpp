#pragma once

// headers
#include "vge_window.hpp"

namespace vge
{

class VgeApp
{
public:
    static constexpr int WIDTH = 800;  // TODO: what for?
    static constexpr int HEIGHT = 600; // TODO: what for?

    void run();

private:
    VgeWindow m_vgeWindow{ WIDTH, HEIGHT, "Hello Vulkan!" }; //  window
};

} // namespace vge

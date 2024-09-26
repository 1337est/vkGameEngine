#pragma once

// libs
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// std
#include <string>

namespace vge
{

class VgeWindow
{
public:
    VgeWindow(int width, int height, std::string name);
    ~VgeWindow();

    // Disable copy constructor and copy assignment operator= to prevent
    // undefined behavior with GLFWwindow* m_window pointer variable
    VgeWindow(const VgeWindow&) = delete;
    VgeWindow& operator=(const VgeWindow&) = delete;

    bool shouldClose() const;

private:
    void initWindow();

    GLFWwindow* m_window;
    int m_width;
    int m_height;
    std::string m_name;
};

} // namespace vge

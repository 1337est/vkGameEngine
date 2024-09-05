#pragma once

// libraries
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// std
#include <string>

namespace vge
{

class VgeWindow
{
public:
    VgeWindow(int width, int height, std::string name); // constructor
    ~VgeWindow();                                       // destructor

    // Disable copy constructor and copy assignment operator= to prevent
    // undefined behavior with GLFWwindow* m_window pointer variable
    VgeWindow(const VgeWindow&) = delete;
    VgeWindow& operator=(const VgeWindow&) = delete;

    bool shouldClose()
    {
        return glfwWindowShouldClose(m_window);
    }

    VkExtent2D getExtent()
    {
        return { static_cast<uint32_t>(m_width),
                 static_cast<uint32_t>(m_height) };
    }

    bool wasWindowResized()
    {
        return m_frameBufferResized;
    }

    void resetWindowResizedFlag()
    {
        m_frameBufferResized = false;
    }

    GLFWwindow* getGLFWwindow() const
    {
        return m_window;
    }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

private: // member variables and functions
    static void frameBufferResizeCallback(
        GLFWwindow* m_window,
        int m_width,
        int m_height);
    void initWindow();
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    bool m_frameBufferResized = false;
    std::string m_name;
};

} // namespace vge

#pragma once

// libs
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// std
#include <string>

namespace vge {

class VgeWindow {
public:
    VgeWindow(int width, int height, std::string name);
    ~VgeWindow();

    // Disable copy constructor and copy assignment operator= to prevent
    // undefined behavior with GLFWwindow* m_window pointer variable
    VgeWindow(const VgeWindow&) = delete;
    VgeWindow& operator=(const VgeWindow&) = delete;

    bool shouldClose() const;

    VkExtent2D getExtent() const;

    bool wasWindowResized() const;

    void resetWindowResizedFlag();

    GLFWwindow* getGLFWwindow() const;

    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

private:
    static void frameBufferResizeCallback(GLFWwindow* window, int newWidth, int newHeight);
    void initWindow();

    GLFWwindow* m_window;
    int m_width;
    int m_height;
    bool m_frameBufferResized = false;
    std::string m_name;
};

} // namespace vge

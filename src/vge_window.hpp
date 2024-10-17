#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace vge {

class VgeWindow {
public:
    VgeWindow();
    ~VgeWindow();

    VgeWindow(const VgeWindow&) = delete;
    VgeWindow& operator=(const VgeWindow&) = delete;

    bool shouldClose() const;
    GLFWwindow* getWindow() const;
    bool wasWindowResized() const;
    void resetWindowResizedFlag();

private:
    static void frameBufferResizeCallback(GLFWwindow* window, int newWidth, int newHeight);
    void initWindow();

    int m_width;
    int m_height;
    std::string m_title;

    GLFWwindow* m_window;
    bool m_frameBufferResized = false;
};

} // namespace vge

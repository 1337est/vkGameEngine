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

private:
    void initWindow();

    int m_width;
    int m_height;
    std::string m_title;

    GLFWwindow* m_window;
};

} // namespace vge

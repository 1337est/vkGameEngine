#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace vge
{

class VgeWindow
{
public:
    VgeWindow(int width, int height, std::string name);
    ~VgeWindow();

    VgeWindow(const VgeWindow&) = delete;
    VgeWindow& operator=(const VgeWindow&) = delete;

    bool shouldClose() const;
    GLFWwindow* getGLFWwindow() const;

private:
    void initWindow();

    GLFWwindow* m_window;
    int m_width;
    int m_height;
    std::string m_name;
};

} // namespace vge

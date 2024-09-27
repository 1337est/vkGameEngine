#pragma once

// libs
#include <vulkan/vulkan.h>

// std
#include <vector>

namespace vge
{

class VgeInstance
{
public:
    VgeInstance();
    ~VgeInstance();

    VgeInstance(const VgeInstance&) = delete;
    VgeInstance& operator=(const VgeInstance&) = delete;

    void createInstance();
    void hasGlfwRequiredInstanceExtensions();
    std::vector<const char*> getRequiredExtensions();

private:
    VkInstance m_instance = VK_NULL_HANDLE;
};
} // namespace vge

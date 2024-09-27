#pragma once

// headers
#include "vge_validation_layers.hpp"

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

private:
    void createInstance();
    void hasGlfwRequiredInstanceExtensions();
    std::vector<const char*> getRequiredExtensions();

    VkInstance m_instance = VK_NULL_HANDLE;
    VgeValidationLayers m_validationLayers;
};
} // namespace vge

#pragma once
// hdrs
#include "vge_validation_layers.hpp"
// libs
#include <vulkan/vulkan.h>
// stds
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

    VkInstance getInstance() const;

private:
    void createInstance();
    void hasGlfwRequiredInstanceExtensions();
    std::vector<const char*> getRequiredExtensions();

    VkInstance m_instance = VK_NULL_HANDLE;
    VgeValidationLayers m_validationLayers;
};
} // namespace vge

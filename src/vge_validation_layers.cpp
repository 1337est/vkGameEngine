#include "vge_validation_layers.hpp"
#include <cstring>
#include <vulkan/vulkan_core.h>

namespace vge {
VgeValidationLayers::VgeValidationLayers()
#ifdef NDEBUG
    : m_enableVLayers{ false }
    , m_VLayers{ "VK_LAYER_KHRONOS_validation" }
#else
    : m_enableVLayers{ true }
    , m_VLayers{ "VK_LAYER_KHRONOS_validation" }
#endif
{
    checkVLayerSupport();
}

void VgeValidationLayers::checkVLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    m_VLayerSupport = true;

    for (const char* layerName : m_VLayers) {
        bool layerFound = false;

        for (const VkLayerProperties& layerProps : availableLayers) {
            if (strcmp(layerName, layerProps.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            m_VLayerSupport = false;
            return;
        }
    }
}

bool VgeValidationLayers::areVLayersEnabled() const
{
    return m_enableVLayers;
}

bool VgeValidationLayers::areVLayersSupported() const
{
    return m_VLayerSupport;
}

const std::vector<const char*>& VgeValidationLayers::getVLayers() const
{
    return m_VLayers;
}

} // namespace vge

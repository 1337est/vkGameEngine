// headers
#include "vge_validation_layers.hpp"

// libs
#include <vulkan/vulkan.h>

// std
#include <cstring>

namespace vge
{
// Initializes validation layers based on build configuration.
VgeValidationLayers::VgeValidationLayers()
#ifdef NDEBUG
    : m_enableValidationLayers{ false }
    , m_validationLayers{ "VK_LAYER_KHRONOS_validation" }
#else
    : m_enableValidationLayers{ true }
    , m_validationLayers{ "VK_LAYER_KHRONOS_validation" }
#endif
{
}

VgeValidationLayers::~VgeValidationLayers() = default;

/* Checks if the requested validation layers are supported
 *
 * This function checks if the required Vulkan validation layers are
 * available.
 */
bool VgeValidationLayers::checkValidationLayerSupport() const
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : m_validationLayers)
    {
        bool layerFound = false;

        for (const VkLayerProperties& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}
} // namespace vge

#pragma once

// std
#include <vector>
#include <vulkan/vulkan.h>

namespace vge
{

class VgeValidationLayers
{
public:
    VgeValidationLayers();

    bool areValidationLayersEnabled() const;
    const std::vector<const char*>& getValidationLayers() const;
    bool checkValidationLayerSupport() const;

    void setupDebugMessenger(const VkInstance& instance);
    void cleanup(const VkInstance& instance);
    void populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT& createInfo);

private:
    VkResult createDebugUtilsMessengerEXT(
        const VkInstance& instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger);

    void destroyDebugUtilsMessengerEXT(
        const VkInstance& instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator);

    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    const std::vector<const char*> m_validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    const bool m_enableValidationLayers;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
};
} // namespace vge

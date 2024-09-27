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
    ~VgeValidationLayers();

    bool areValidationLayersEnabled() const
    {
        return m_enableValidationLayers;
    }

    const std::vector<const char*>& getValidationLayers() const
    {
        return m_validationLayers;
    }

    bool checkValidationLayerSupport() const;

    void setInstance(VkInstance instance);
    void cleanup();
    void setupDebugMessenger();

    void populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT& createInfo);

private:
    VkResult createDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger);

    void destroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator);

    VkInstance m_instance = VK_NULL_HANDLE;
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

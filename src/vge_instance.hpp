#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace vge {

class VgeInstance {
public:
    VgeInstance();
    ~VgeInstance();

    VgeInstance(const VgeInstance&) = delete;
    VgeInstance& operator=(const VgeInstance&) = delete;

    bool areValidationLayersEnabled() const;
    const std::vector<const char*>& getValidationLayers() const;
    bool checkValidationLayerSupport() const;

    VkInstance getInstance() const;

private:
    void createInstance();
    void hasGlfwRequiredInstanceExtensions();
    std::vector<const char*> getRequiredExtensions();

    void setupDebugMessenger();
    void populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    VkResult createDebugUtilsMessengerEXT(
        const VkInstance& instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger);
    void destroyDebugUtilsMessengerEXT(
        const VkInstance& instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    VkInstance m_instance = VK_NULL_HANDLE;

    const bool m_enableValidationLayers;
    const std::vector<const char*> m_validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
};
} // namespace vge

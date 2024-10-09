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

    bool areVLayersEnabled() const;
    const std::vector<const char*>& getVLayers() const;

    VkInstance getInstance() const;

private:
    void setRequiredExts();
    void checkVLayerSupport();
    void hasRequiredInstanceExts();
    void createInstance();
    void setupDebugMessenger();

    void populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& debugCI);
    VkResult createDebugMessenger(
        const VkInstance& instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pDebugCI,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger);
    void destroyDebugMessenger(
        const VkInstance& instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    VkInstance m_instance = VK_NULL_HANDLE;

    const bool m_enableVLayers;
    const std::vector<const char*> m_VLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    bool m_VLayerSupport;

    std::vector<const char*> m_requiredExts;

    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    VkDebugUtilsMessengerCreateInfoEXT m_debugCI;
};
} // namespace vge

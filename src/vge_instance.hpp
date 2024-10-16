#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace vge {

class VgeInstance {
public:
    VgeInstance(bool enableVLayers, bool vLayerSupport, std::vector<const char*> VLayers);
    ~VgeInstance();

    VgeInstance(const VgeInstance&) = delete;
    VgeInstance& operator=(const VgeInstance&) = delete;

    VkInstance getInstance() const;

private:
    void setRequiredExts();
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
    bool m_VLayerSupport;
    const std::vector<const char*> m_VLayers;

    std::vector<const char*> m_requiredExts;

    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    VkDebugUtilsMessengerCreateInfoEXT m_debugCI;
};
} // namespace vge

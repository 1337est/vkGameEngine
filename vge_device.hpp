#pragma once

// headers
#include "vge_window.hpp"

// std
#include <string>
#include <vector>

namespace vge
{

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    SwapChainSupportDetails()
        : capabilities{}
        , formats{}
        , presentModes{}
    {
    }
};

struct QueueFamilyIndices
{
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool graphicsFamilyHasValue = false;
    bool presentFamilyHasValue = false;

    QueueFamilyIndices()
        : graphicsFamily{ 0 }
        , presentFamily{ 0 }
    {
    }

    bool isComplete()
    {
        return graphicsFamilyHasValue && presentFamilyHasValue;
    }
};

class VgeDevice
{
public:
#ifdef NDEBUG
    const bool m_enableValidationLayers = false;
#else
    const bool m_enableValidationLayers = true;
#endif

    VgeDevice(VgeWindow& window);
    ~VgeDevice();

    // Not copyable or movable
    VgeDevice(const VgeDevice&) = delete;
    VgeDevice& operator=(const VgeDevice&) = delete;
    VgeDevice(VgeDevice&&) = delete;
    VgeDevice& operator=(VgeDevice&&) = delete;

    VkCommandPool getCommandPool()
    {
        return m_commandPool;
    }

    VkDevice device()
    {
        return m_device_;
    }

    VkSurfaceKHR surface()
    {
        return m_surface_;
    }

    VkQueue graphicsQueue()
    {
        return m_graphicsQueue_;
    }

    VkQueue presentQueue()
    {
        return m_presentQueue_;
    }

    SwapChainSupportDetails getSwapChainSupport()
    {
        return querySwapChainSupport(m_physicalDevice);
    }

    uint32_t findMemoryType(
        uint32_t typeFilter,
        VkMemoryPropertyFlags properties);

    QueueFamilyIndices findPhysicalQueueFamilies()
    {
        return findQueueFamilies(m_physicalDevice);
    }

    VkFormat findSupportedFormat(
        const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features);

    // Buffer Helper Functions
    void createBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void copyBufferToImage(
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height,
        uint32_t layerCount);

    void createImageWithInfo(
        const VkImageCreateInfo& imageInfo,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMemory);

    VkPhysicalDeviceProperties m_properties;

private:
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool();

    // helper functions
    bool isDeviceSuitable(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    void populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void hasGflwRequiredInstanceExtensions();
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VgeWindow& m_window;
    VkCommandPool m_commandPool;

    VkDevice m_device_;
    VkSurfaceKHR m_surface_;
    VkQueue m_graphicsQueue_;
    VkQueue m_presentQueue_;

    const std::vector<const char*> m_validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const std::vector<const char*> m_deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};

} // namespace vge

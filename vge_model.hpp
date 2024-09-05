#pragma once

// headers
#include "vge_device.hpp"

// libraries
#include <vulkan/vulkan_core.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>

namespace vge
{

class VgeModel
{
public:
    struct Vertex
    {
        glm::vec3 position{};
        glm::vec3 color{};
        glm::vec3 normal{};
        glm::vec2 uv{}; // shorthand for 2d texture coordinate

        static std::vector<VkVertexInputBindingDescription>
        getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription>
        getAttributeDescriptions();
    };

    struct Builder
    {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};

        void loadModel(const std::string& filepath);
    };

    VgeModel(VgeDevice& device, const VgeModel::Builder& builder);
    ~VgeModel();

    VgeModel(const VgeModel&) = delete;
    VgeModel& operator=(const VgeModel&) = delete;

    static std::unique_ptr<VgeModel> createModelFromFile(
        VgeDevice& device,
        const std::string& filepath);

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

private:
    void createVertexBuffers(const std::vector<Vertex>& vertices);
    void createIndexBuffers(const std::vector<uint32_t>& indices);

    VgeDevice& m_vgeDevice;

    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    uint32_t m_vertexCount;

    bool m_hasIndexBuffer = false;
    VkBuffer m_indexBuffer;
    VkDeviceMemory m_indexBufferMemory;
    uint32_t m_indexCount;
};

} // namespace vge

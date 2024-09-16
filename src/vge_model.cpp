// headers
#include "vge_model.hpp"
#include "vge_utils.hpp"

// libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <vulkan/vulkan_core.h>

// std
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <unordered_map>

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif

namespace std
{
template <> struct hash<vge::VgeModel::Vertex>
{
    size_t operator()(vge::VgeModel::Vertex const& vertex) const
    {
        size_t seed = 0;
        vge::hashCombine(
            seed,
            vertex.position,
            vertex.color,
            vertex.normal,
            vertex.uv);
        return seed;
    }
};
} // namespace std

namespace vge
{

VgeModel::VgeModel(VgeDevice& device, const VgeModel::Builder& builder)
    : m_vgeDevice{ device }
    , m_vertexBuffer{}
    , m_vertexCount{}
    , m_indexBuffer{}
    , m_indexCount{}
{
    createVertexBuffers(builder.vertices);
    createIndexBuffers(builder.indices);
}

VgeModel::~VgeModel()
{
}

std::unique_ptr<VgeModel> VgeModel::createModelFromFile(
    VgeDevice& device,
    const std::string& filepath)
{
    Builder builder{};
    builder.loadModel(ENGINE_DIR + filepath);

    return std::make_unique<VgeModel>(device, builder);
}

void VgeModel::createVertexBuffers(const std::vector<Vertex>& vertices)
{
    m_vertexCount = static_cast<uint32_t>(vertices.size());
    assert(m_vertexCount >= 3 && "Vertex count must be at least 3");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;
    uint32_t vertexSize = sizeof(vertices[0]);

    VgeBuffer stagingBuffer{
        m_vgeDevice,
        vertexSize,
        m_vertexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)vertices.data());

    m_vertexBuffer = std::make_unique<VgeBuffer>(
        m_vgeDevice,
        vertexSize,
        m_vertexCount,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT

    );

    m_vgeDevice.copyBuffer(
        stagingBuffer.getBuffer(),
        m_vertexBuffer->getBuffer(),
        bufferSize);
}

void VgeModel::createIndexBuffers(const std::vector<uint32_t>& indices)
{
    m_indexCount = static_cast<uint32_t>(indices.size());
    m_hasIndexBuffer = m_indexCount > 0;

    if (!m_hasIndexBuffer)
    {
        return;
    }

    VkDeviceSize bufferSize = sizeof(indices[0]) * m_indexCount;
    uint32_t indexSize = sizeof(indices[0]);

    VgeBuffer stagingBuffer{
        m_vgeDevice,
        indexSize,
        m_indexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)indices.data());

    m_indexBuffer = std::make_unique<VgeBuffer>(
        m_vgeDevice,
        indexSize,
        m_indexCount,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_vgeDevice.copyBuffer(
        stagingBuffer.getBuffer(),
        m_indexBuffer->getBuffer(),
        bufferSize);
}

void VgeModel::draw(VkCommandBuffer commandBuffer)
{
    if (m_hasIndexBuffer)
    {
        vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
    }
    else
    {
        vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
    }
}

void VgeModel::bind(VkCommandBuffer commandBuffer)
{
    VkBuffer buffers[] = { m_vertexBuffer->getBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    if (m_hasIndexBuffer)
    {
        vkCmdBindIndexBuffer(
            commandBuffer,
            m_indexBuffer->getBuffer(),
            0,
            VK_INDEX_TYPE_UINT32);
    }
}

std::vector<VkVertexInputBindingDescription> VgeModel::Vertex::
    getBindingDescriptions()
{
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> VgeModel::Vertex::
    getAttributeDescriptions()
{
    // location, binding, format, offset
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

    attributeDescriptions.push_back(
        { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
    attributeDescriptions.push_back(
        { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
    attributeDescriptions.push_back(
        { 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
    attributeDescriptions.push_back(
        { 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

    return attributeDescriptions;
}

void VgeModel::Builder::loadModel(const std::string& filepath)
{
    // All of these values will be set by tinyobjloader and will store the
    // results of reading a wavefront .obj file
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // initialize above variables with data from .obj file
    if (!tinyobj::LoadObj(
            &attrib, // stores pos, color, normal, texture coord data
            &shapes, // index values for each face element
            &materials,
            &warn,
            &err,
            filepath.c_str()))
    {
        throw std::runtime_error(warn + err);
    }

    vertices.clear();
    indices.clear();

    // map to store already added vertices to the Builder.vertices vector
    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    // loop through each face
    for (const tinyobj::shape_t& shape : shapes)
    {
        for (const tinyobj::index_t& index : shape.mesh.indices)
        {
            Vertex vertex{};

            if (index.vertex_index >= 0)
            {
                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0], // x-pos
                    attrib.vertices[3 * index.vertex_index + 1], // y-pos
                    attrib.vertices[3 * index.vertex_index + 2], // z-pos
                };

                // if RGB values are present in .obj file
                vertex.color = {
                    attrib.colors[3 * index.vertex_index + 0],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2],
                };
            }
            if (index.normal_index >= 0)
            {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2],
                };
            }
            if (index.texcoord_index >= 0)
            {
                vertex.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1],
                };
            }

            // if new, add to the map
            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            // add pos of vertex to the Builders indices vector
            indices.push_back(uniqueVertices[vertex]);
        }
    }
}
} // namespace vge

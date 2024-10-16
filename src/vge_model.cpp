#include "vge_model.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstring>
#include <stdexcept>
#include <unordered_map>

namespace std {
/* Specializes the hash function for vge::VgeModel::Vertex.
 *
 * This struct provides a custom hash function for Vertex objects, combining
 * the hash values of the position, color, normal, and UV coordinates to
 * produce a unique hash value for each Vertex, enabling its use in hash-based
 * containers like std::unordered_map.
 */
template <> struct hash<vge::VgeModel::Vertex>
{
    size_t operator()(const vge::VgeModel::Vertex& vertex) const
    {
        size_t seed = 0;
        vge::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
        return seed;
    }
};
} // namespace std

namespace vge {

/* Combines multiple hash values into a single hash seed.
 *
 * This function updates the provided seed with the hash of the given values,
 * allowing for the efficient hashing of multiple components by using a
 * combination algorithm.
 */
template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest)
{
    seed ^= std::hash<T>{}(v) + 0x9e'37'79'b9 + (seed << 6) + (seed >> 2);
    // C++17 fold expression (f(), ...) to hash remaining element in rest...
    (hashCombine(seed, rest), ...);
}

/* Constructs a VgeModel from the given device and builder.
 *
 * This constructor initializes vertex and index buffers by calling the
 * respective creation methods with data from the builder.
 */
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

/* Cleans up resources associated with the VgeModel.
 *
 * This destructor ensures that all allocated resources are properly released.
 */
VgeModel::~VgeModel()
{}

/* Compares two Vertex instances for equality.
 *
 * This method checks if the position, color, normal, and UV coordinates
 * of two Vertex objects are identical.
 */
bool VgeModel::Vertex::operator==(const Vertex& other) const
{
    return position == other.position && color == other.color && normal == other.normal &&
           uv == other.uv;
}

/* Creates a VgeModel instance from a specified file.
 *
 * This static method loads a model from the given file path and returns
 * a unique pointer to the created VgeModel instance.
 */
std::unique_ptr<VgeModel> VgeModel::createModelFromFile(
    VgeDevice& device,
    const std::string& filepath)
{
    Builder builder{};
    builder.loadModel(filepath);

    return std::make_unique<VgeModel>(device, builder);
}

/* Creates vertex buffers for the model from the provided vertices.
 *
 * This method allocates a staging buffer, maps it, and copies vertex data
 * into the GPU-usable vertex buffer.
 */
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
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
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

    m_vgeDevice.copyBuffer(stagingBuffer.getBuffer(), m_vertexBuffer->getBuffer(), bufferSize);
}

/* Creates index buffers for the model from the provided indices.
 *
 * This method allocates a staging buffer for index data, maps it, and
 * copies the indices into the GPU-usable index buffer, if any indices are
 * provided.
 */
void VgeModel::createIndexBuffers(const std::vector<uint32_t>& indices)
{
    m_indexCount = static_cast<uint32_t>(indices.size());
    m_hasIndexBuffer = m_indexCount > 0;

    if (!m_hasIndexBuffer) {
        return;
    }

    VkDeviceSize bufferSize = sizeof(indices[0]) * m_indexCount;
    uint32_t indexSize = sizeof(indices[0]);

    VgeBuffer stagingBuffer{
        m_vgeDevice,
        indexSize,
        m_indexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)indices.data());

    m_indexBuffer = std::make_unique<VgeBuffer>(
        m_vgeDevice,
        indexSize,
        m_indexCount,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_vgeDevice.copyBuffer(stagingBuffer.getBuffer(), m_indexBuffer->getBuffer(), bufferSize);
}

/* Draws the model using the specified command buffer.
 *
 * This method issues a draw call, either indexed or non-indexed, depending
 * on the presence of an index buffer.
 */
void VgeModel::draw(VkCommandBuffer commandBuffer)
{
    if (m_hasIndexBuffer) {
        vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
    }
    else {
        vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
    }
}

/* Binds the vertex and index buffers to the specified command buffer.
 *
 * This method sets up the buffers for rendering, ensuring they are bound
 * to the correct slots in the command buffer.
 */
void VgeModel::bind(VkCommandBuffer commandBuffer)
{
    VkBuffer buffers[] = { m_vertexBuffer->getBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    if (m_hasIndexBuffer) {
        vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
}

/* Retrieves the vertex input binding descriptions for the model.
 *
 * This method returns a vector of binding descriptions required for
 * configuring vertex input in the graphics pipeline.
 */
std::vector<VkVertexInputBindingDescription> VgeModel::Vertex::getBindingDescriptions()
{
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
}

/* Retrieves the vertex input attribute descriptions for the model.
 *
 * This method returns a vector of attribute descriptions that define the
 * layout of vertex data in the graphics pipeline.
 */
std::vector<VkVertexInputAttributeDescription> VgeModel::Vertex::getAttributeDescriptions()
{
    // location, binding, format, offset
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

    attributeDescriptions.push_back(
        { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
    attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
    attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
    attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

    return attributeDescriptions;
}

/* Loads a model from the specified file into the builder.
 *
 * This method reads a Wavefront .obj file, extracts vertex and index data,
 * and stores it in the builder's vertices and indices vectors.
 */
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
    for (const tinyobj::shape_t& shape : shapes) {
        for (const tinyobj::index_t& index : shape.mesh.indices) {
            Vertex vertex{};

            if (index.vertex_index >= 0) {
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
            if (index.normal_index >= 0) {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2],
                };
            }
            if (index.texcoord_index >= 0) {
                vertex.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1],
                };
            }

            // if new, add to the map
            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            // add pos of vertex to the Builders indices vector
            indices.push_back(uniqueVertices[vertex]);
        }
    }
}
} // namespace vge

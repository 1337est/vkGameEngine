/* Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */

// headers
#include "vge_buffer.hpp"

// std
#include <cassert>
#include <cstring>

namespace vge
{

/* Returns the minimum instance size required to be compatible with devices
 * minOffsetAlignment
 *
 * This function calculates and returns the minimum size of an instance based on
 * the required alignment and the size of the instance.
 */
VkDeviceSize VgeBuffer::getAlignment(
    VkDeviceSize instanceSize,
    VkDeviceSize minOffsetAlignment)
{
    if (minOffsetAlignment > 0)
    {
        return (instanceSize + minOffsetAlignment - 1) &
               ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}

/* Constructs a Vulkan buffer with specified properties.
 *
 * This constructor initializes the buffer with a specified instance size,
 * instance count, usage flags, memory properties, and alignment, and creates
 * the Vulkan buffer and associated memory.
 */
VgeBuffer::VgeBuffer(
    VgeDevice& device,
    VkDeviceSize instanceSize,
    uint32_t instanceCount,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkDeviceSize minOffsetAlignment)
    : m_vgeDevice{ device }
    , m_bufferSize{}
    , m_instanceSize{ instanceSize }
    , m_instanceCount{ instanceCount }
    , m_usageFlags{ usageFlags }
    , m_memoryPropertyFlags{ memoryPropertyFlags }
    , m_alignmentSize{}
{
    m_alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
    m_bufferSize = m_alignmentSize * instanceCount;
    device.createBuffer(
        m_bufferSize,
        usageFlags,
        memoryPropertyFlags,
        m_buffer,
        m_memory);
}

/* Cleans up resources associated with the Vulkan buffer.
 *
 * This destructor unmaps the buffer memory, destroys the Vulkan buffer,
 * and frees the allocated memory.
 */
VgeBuffer::~VgeBuffer()
{
    unmap();
    vkDestroyBuffer(m_vgeDevice.device(), m_buffer, nullptr);
    vkFreeMemory(m_vgeDevice.device(), m_memory, nullptr);
}

/* Maps a memory range of this buffer. If successful, mapped points to the
 * specified buffer range.
 *
 * This function maps a specified range of the buffer memory to the host,
 * allowing access to the buffer data.
 */
VkResult VgeBuffer::map(VkDeviceSize size, VkDeviceSize offset)
{
    assert(m_buffer && m_memory && "Called map on buffer before create");
    return vkMapMemory(
        m_vgeDevice.device(),
        m_memory,
        offset,
        size,
        0,
        &m_mapped);
}

/* Unmaps a mapped memory range.
 *
 * This function unmaps the previously mapped buffer memory range,
 * making it inaccessible from the host.
 */
void VgeBuffer::unmap()
{
    if (m_mapped)
    {
        vkUnmapMemory(m_vgeDevice.device(), m_memory);
        m_mapped = nullptr;
    }
}

/* Copies the specified data to the mapped buffer. Default value writes the
 * whole buffer range.
 *
 * This function copies data from the specified pointer to the mapped buffer,
 * allowing for updating the buffer contents.
 */
void VgeBuffer::writeToBuffer(
    void* data,
    VkDeviceSize size,
    VkDeviceSize offset)
{
    assert(m_mapped && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE)
    {
        memcpy(m_mapped, data, m_bufferSize);
    }
    else
    {
        char* memOffset = (char*)m_mapped;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}

/* Flushes a memory range of the buffer to make it visible to the device.
 *
 * This function ensures that the modified data in the specified memory range
 * is visible to the GPU, particularly for non-coherent memory.
 */
VkResult VgeBuffer::flush(VkDeviceSize size, VkDeviceSize offset)
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = m_memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(m_vgeDevice.device(), 1, &mappedRange);
}

/* Invalidates a memory range of the buffer to make it visible to the host.
 *
 * This function invalidates the specified memory range, ensuring that the
 * host sees the most recent data from the device, particularly for non-coherent
 * memory.
 */
VkResult VgeBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = m_memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(
        m_vgeDevice.device(),
        1,
        &mappedRange);
}

/* Creates a buffer info descriptor.
 *
 * This function returns a VkDescriptorBufferInfo structure for the buffer,
 * specifying the size and offset for descriptor sets.
 */
VkDescriptorBufferInfo VgeBuffer::descriptorInfo(
    VkDeviceSize size,
    VkDeviceSize offset)
{
    return VkDescriptorBufferInfo{
        m_buffer,
        offset,
        size,
    };
}

/* Copies instanceSize bytes of data to the mapped buffer at an offset of
 * index * alignmentSize.
 *
 * This function writes data to a specific index in the buffer, allowing
 * for instance-specific data updates.
 */
void VgeBuffer::writeToIndex(void* data, int index)
{
    writeToBuffer(data, m_instanceSize, index * m_alignmentSize);
}

/* Flushes the memory range at index * alignmentSize of the buffer to make it
 * visible to the device.
 *
 * This function ensures that the modified data at a specific index is
 * visible to the GPU, particularly for non-coherent memory.
 */
VkResult VgeBuffer::flushIndex(int index)
{
    return flush(m_alignmentSize, index * m_alignmentSize);
}

/* Creates a buffer info descriptor for a specific index.
 *
 * This function returns a VkDescriptorBufferInfo structure for the buffer
 * at the specified index, indicating the offset and size for descriptor sets.
 */
VkDescriptorBufferInfo VgeBuffer::descriptorInfoForIndex(int index)
{
    return descriptorInfo(m_alignmentSize, index * m_alignmentSize);
}

/* Invalidates a memory range of the buffer to make it visible to the host.
 *
 * This function invalidates the memory range at a specific index, ensuring
 * that the host sees the most recent data from the device, particularly for
 * non-coherent memory.
 */
VkResult VgeBuffer::invalidateIndex(int index)
{
    return invalidate(m_alignmentSize, index * m_alignmentSize);
}

/* Retrieves the Vulkan buffer handle.
 *
 * This function returns the handle to the Vulkan buffer, allowing access
 * to the buffer in Vulkan API calls.
 */
VkBuffer VgeBuffer::getBuffer() const
{
    return m_buffer;
}

/* Retrieves the mapped memory pointer.
 *
 * This function returns the pointer to the mapped memory range of the buffer,
 * allowing access to the data in the buffer.
 */
void* VgeBuffer::getMappedMemory() const
{
    return m_mapped;
}

/* Retrieves the instance count for the buffer.
 *
 * This function returns the number of instances that the buffer can hold.
 */
uint32_t VgeBuffer::getInstanceCount() const
{
    return m_instanceCount;
}

/* Retrieves the instance size for the buffer.
 *
 * This function returns the size of an individual instance in the buffer.
 */
VkDeviceSize VgeBuffer::getInstanceSize() const
{
    return m_instanceSize;
}

/* Retrieves the alignment size for the buffer.
 *
 * This function returns the alignment size required for the buffer instances.
 */
VkDeviceSize VgeBuffer::getAlignmentSize() const
{
    return m_instanceSize;
}

/* Retrieves the usage flags for the buffer.
 *
 * This function returns the usage flags that specify how the buffer can be used
 * in Vulkan operations.
 */
VkBufferUsageFlags VgeBuffer::getUsageFlags() const
{
    return m_usageFlags;
}

/* Retrieves the memory property flags for the buffer.
 *
 * This function returns the memory property flags that specify the memory
 * allocation properties of the buffer.
 */
VkMemoryPropertyFlags VgeBuffer::getMemoryPropertyFlags() const
{
    return m_memoryPropertyFlags;
}

/* Retrieves the size of the buffer.
 *
 * This function returns the total size of the buffer in bytes.
 */
VkDeviceSize VgeBuffer::getBufferSize() const
{
    return m_bufferSize;
}
} // namespace vge

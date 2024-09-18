/*
 * Encapsulates a vulkan buffer
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

/**
 * Returns the minimum instance size required to be compatible with devices
 * minOffsetAlignment
 *
 * @param instanceSize The size of an instance
 * @param minOffsetAlignment The minimum required alignment, in bytes, for the
 * offset member (eg minUniformBufferOffsetAlignment)
 *
 * @return VkResult of the buffer mapping call
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

VgeBuffer::~VgeBuffer()
{
    unmap();
    vkDestroyBuffer(m_vgeDevice.device(), m_buffer, nullptr);
    vkFreeMemory(m_vgeDevice.device(), m_memory, nullptr);
}

/**
 * Map a memory range of this buffer. If successful, mapped points to the
 * specified buffer range.
 *
 * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to
 * map the complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the buffer mapping call
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

/**
 * Unmap a mapped memory range
 *
 * @note Does not return a result as vkUnmapMemory can't fail
 */
void VgeBuffer::unmap()
{
    if (m_mapped)
    {
        vkUnmapMemory(m_vgeDevice.device(), m_memory);
        m_mapped = nullptr;
    }
}

/**
 * Copies the specified data to the mapped buffer. Default value writes whole
 * buffer range
 *
 * @param data Pointer to the data to copy
 * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush
 * the complete buffer range.
 * @param offset (Optional) Byte offset from beginning of mapped region
 *
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

/**
 * Flush a memory range of the buffer to make it visible to the device
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE
 * to flush the complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the flush call
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

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to invalidate. Pass
 * VK_WHOLE_SIZE to invalidate the complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the invalidate call
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

/**
 * Create a buffer info descriptor
 *
 * @param size (Optional) Size of the memory range of the descriptor
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkDescriptorBufferInfo of specified offset and range
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

/**
 * Copies "instanceSize" bytes of data to the mapped buffer at an offset of
 * index * alignmentSize
 *
 * @param data Pointer to the data to copy
 * @param index Used in offset calculation
 *
 */
void VgeBuffer::writeToIndex(void* data, int index)
{
    writeToBuffer(data, m_instanceSize, index * m_alignmentSize);
}

/**
 *  Flush the memory range at index * alignmentSize of the buffer to make it
 * visible to the device
 *
 * @param index Used in offset calculation
 *
 */
VkResult VgeBuffer::flushIndex(int index)
{
    return flush(m_alignmentSize, index * m_alignmentSize);
}

/**
 * Create a buffer info descriptor
 *
 * @param index Specifies the region given by index * alignmentSize
 *
 * @return VkDescriptorBufferInfo for instance at index
 */
VkDescriptorBufferInfo VgeBuffer::descriptorInfoForIndex(int index)
{
    return descriptorInfo(m_alignmentSize, index * m_alignmentSize);
}

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param index Specifies the region to invalidate: index * alignmentSize
 *
 * @return VkResult of the invalidate call
 */
VkResult VgeBuffer::invalidateIndex(int index)
{
    return invalidate(m_alignmentSize, index * m_alignmentSize);
}

VkBuffer VgeBuffer::getBuffer() const
{
    return m_buffer;
}

void* VgeBuffer::getMappedMemory() const
{
    return m_mapped;
}

uint32_t VgeBuffer::getInstanceCount() const
{
    return m_instanceCount;
}

VkDeviceSize VgeBuffer::getInstanceSize() const
{
    return m_instanceSize;
}

VkDeviceSize VgeBuffer::getAlignmentSize() const
{
    return m_instanceSize;
}

VkBufferUsageFlags VgeBuffer::getUsageFlags() const
{
    return m_usageFlags;
}

VkMemoryPropertyFlags VgeBuffer::getMemoryPropertyFlags() const
{
    return m_memoryPropertyFlags;
}

VkDeviceSize VgeBuffer::getBufferSize() const
{
    return m_bufferSize;
}
} // namespace vge

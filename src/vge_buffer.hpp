#pragma once

#include "vge_device.hpp"

namespace vge {

class VgeBuffer {
public:
    VgeBuffer(
        VgeDevice& device,
        VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkDeviceSize minOffsetAlignment = 1);
    ~VgeBuffer();

    VgeBuffer(const VgeBuffer&) = delete;
    VgeBuffer& operator=(const VgeBuffer&) = delete;

    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void unmap();

    void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    VkDescriptorBufferInfo descriptorInfo(
        VkDeviceSize size = VK_WHOLE_SIZE,
        VkDeviceSize offset = 0);

    VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    void writeToIndex(void* data, int index);
    VkResult flushIndex(int index);
    VkDescriptorBufferInfo descriptorInfoForIndex(int index);
    VkResult invalidateIndex(int index);

    VkBuffer getBuffer() const;
    void* getMappedMemory() const;
    uint32_t getInstanceCount() const;
    VkDeviceSize getInstanceSize() const;
    VkDeviceSize getAlignmentSize() const;
    VkBufferUsageFlags getUsageFlags() const;
    VkMemoryPropertyFlags getMemoryPropertyFlags() const;
    VkDeviceSize getBufferSize() const;

private:
    static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

    VgeDevice& m_vgeDevice;
    void* m_mapped = nullptr;
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;

    VkDeviceSize m_bufferSize;
    VkDeviceSize m_instanceSize;
    uint32_t m_instanceCount;
    VkBufferUsageFlags m_usageFlags;
    VkMemoryPropertyFlags m_memoryPropertyFlags;
    VkDeviceSize m_alignmentSize;
};

} // namespace vge

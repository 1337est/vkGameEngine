#pragma once

// headers
#include "vge_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace vge
{

class VgeDescriptorSetLayout
{
public:
    class Builder
    {
    public:
        Builder(VgeDevice& vgeDevice)
            : m_vgeDevice{ vgeDevice }
        {
        }

        Builder& addBinding(
            uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stageFlags,
            uint32_t count = 1);
        std::unique_ptr<VgeDescriptorSetLayout> build() const;

    private:
        VgeDevice& m_vgeDevice;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings{};
    };

    VgeDescriptorSetLayout(
        VgeDevice& vgeDevice,
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~VgeDescriptorSetLayout();
    VgeDescriptorSetLayout(const VgeDescriptorSetLayout&) = delete;
    VgeDescriptorSetLayout& operator=(const VgeDescriptorSetLayout&) = delete;

    VkDescriptorSetLayout getDescriptorSetLayout() const;

private:
    VgeDevice& m_vgeDevice;
    VkDescriptorSetLayout m_descriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;

    friend class VgeDescriptorWriter;
};

class VgeDescriptorPool
{
public:
    class Builder
    {
    public:
        Builder(VgeDevice& vgeDevice)
            : m_vgeDevice{ vgeDevice }
        {
        }

        Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
        Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
        Builder& setMaxSets(uint32_t count);
        std::unique_ptr<VgeDescriptorPool> build() const;

    private:
        VgeDevice& m_vgeDevice;
        std::vector<VkDescriptorPoolSize> m_poolSizes{};
        uint32_t m_maxSets = 1000;
        VkDescriptorPoolCreateFlags m_poolFlags = 0;
    };

    VgeDescriptorPool(
        VgeDevice& vgeDevice,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes);
    ~VgeDescriptorPool();
    VgeDescriptorPool(const VgeDescriptorPool&) = delete;
    VgeDescriptorPool& operator=(const VgeDescriptorPool&) = delete;

    bool allocateDescriptorSet(
        const VkDescriptorSetLayout descriptorSetLayout,
        VkDescriptorSet& descriptor) const;

    void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

    void resetPool();

private:
    VgeDevice& m_vgeDevice;
    VkDescriptorPool m_descriptorPool;

    friend class VgeDescriptorWriter;
};

class VgeDescriptorWriter
{
public:
    VgeDescriptorWriter(
        VgeDescriptorSetLayout& setLayout,
        VgeDescriptorPool& pool);

    VgeDescriptorWriter& writeBuffer(
        uint32_t binding,
        VkDescriptorBufferInfo* bufferInfo);
    VgeDescriptorWriter& writeImage(
        uint32_t binding,
        VkDescriptorImageInfo* imageInfo);

    bool build(VkDescriptorSet& set);
    void overwrite(VkDescriptorSet& set);

private:
    VgeDescriptorSetLayout& m_setLayout;
    VgeDescriptorPool& m_pool;
    std::vector<VkWriteDescriptorSet> m_writes;
};

} // namespace vge

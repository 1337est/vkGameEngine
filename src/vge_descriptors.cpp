// headers
#include "vge_descriptors.hpp"

// std
#include <cassert>
#include <stdexcept>

namespace vge
{

// *************** Descriptor Set Layout Builder *********************

VgeDescriptorSetLayout::Builder& VgeDescriptorSetLayout::Builder::addBinding(
    uint32_t binding,
    VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags,
    uint32_t count)
{
    assert(m_bindings.count(binding) == 0 && "Binding already in use");
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;
    m_bindings[binding] = layoutBinding;
    return *this;
}

std::unique_ptr<VgeDescriptorSetLayout> VgeDescriptorSetLayout::Builder::build()
    const
{
    return std::make_unique<VgeDescriptorSetLayout>(m_vgeDevice, m_bindings);
}

// *************** Descriptor Set Layout *********************

VgeDescriptorSetLayout::VgeDescriptorSetLayout(
    VgeDevice& vgeDevice,
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : m_vgeDevice{ vgeDevice }
    , m_bindings{ bindings }
{
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
    for (std::pair<const unsigned int, VkDescriptorSetLayoutBinding> kv :
         bindings)
    {
        setLayoutBindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount =
        static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(
            vgeDevice.device(),
            &descriptorSetLayoutInfo,
            nullptr,
            &m_descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
}

VgeDescriptorSetLayout::~VgeDescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(
        m_vgeDevice.device(),
        m_descriptorSetLayout,
        nullptr);
}

VkDescriptorSetLayout VgeDescriptorSetLayout::getDescriptorSetLayout() const
{
    return m_descriptorSetLayout;
}

// *************** Descriptor Pool Builder *********************

VgeDescriptorPool::Builder& VgeDescriptorPool::Builder::addPoolSize(
    VkDescriptorType descriptorType,
    uint32_t count)
{
    m_poolSizes.push_back({ descriptorType, count });
    return *this;
}

VgeDescriptorPool::Builder& VgeDescriptorPool::Builder::setPoolFlags(
    VkDescriptorPoolCreateFlags flags)
{
    m_poolFlags = flags;
    return *this;
}

VgeDescriptorPool::Builder& VgeDescriptorPool::Builder::setMaxSets(
    uint32_t count)
{
    m_maxSets = count;
    return *this;
}

std::unique_ptr<VgeDescriptorPool> VgeDescriptorPool::Builder::build() const
{
    return std::make_unique<VgeDescriptorPool>(
        m_vgeDevice,
        m_maxSets,
        m_poolFlags,
        m_poolSizes);
}

// *************** Descriptor Pool *********************

VgeDescriptorPool::VgeDescriptorPool(
    VgeDevice& vgeDevice,
    uint32_t maxSets,
    VkDescriptorPoolCreateFlags poolFlags,
    const std::vector<VkDescriptorPoolSize>& poolSizes)
    : m_vgeDevice{ vgeDevice }
{
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = poolFlags;

    if (vkCreateDescriptorPool(
            vgeDevice.device(),
            &descriptorPoolInfo,
            nullptr,
            &m_descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
}

VgeDescriptorPool::~VgeDescriptorPool()
{
    vkDestroyDescriptorPool(m_vgeDevice.device(), m_descriptorPool, nullptr);
}

bool VgeDescriptorPool::allocateDescriptorSet(
    const VkDescriptorSetLayout descriptorSetLayout,
    VkDescriptorSet& descriptor) const
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    allocInfo.descriptorSetCount = 1;

    // Might want to create a "DescriptorPoolManager" class that handles this
    // case, and builds a new pool whenever an old pool fills up. But this is
    // beyond our current scope
    if (vkAllocateDescriptorSets(
            m_vgeDevice.device(),
            &allocInfo,
            &descriptor) != VK_SUCCESS)
    {
        return false;
    }
    return true;
}

void VgeDescriptorPool::freeDescriptors(
    std::vector<VkDescriptorSet>& descriptors) const
{
    vkFreeDescriptorSets(
        m_vgeDevice.device(),
        m_descriptorPool,
        static_cast<uint32_t>(descriptors.size()),
        descriptors.data());
}

void VgeDescriptorPool::resetPool()
{
    vkResetDescriptorPool(m_vgeDevice.device(), m_descriptorPool, 0);
}

// *************** Descriptor Writer *********************

VgeDescriptorWriter::VgeDescriptorWriter(
    VgeDescriptorSetLayout& setLayout,
    VgeDescriptorPool& pool)
    : m_setLayout{ setLayout }
    , m_pool{ pool }
{
}

VgeDescriptorWriter& VgeDescriptorWriter::writeBuffer(
    uint32_t binding,
    VkDescriptorBufferInfo* bufferInfo)
{
    assert(
        m_setLayout.m_bindings.count(binding) == 1 &&
        "Layout does not contain specified binding");

    VkDescriptorSetLayoutBinding& bindingDescription =
        m_setLayout.m_bindings[binding];

    assert(
        bindingDescription.descriptorCount == 1 &&
        "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = bufferInfo;
    write.descriptorCount = 1;

    m_writes.push_back(write);
    return *this;
}

VgeDescriptorWriter& VgeDescriptorWriter::writeImage(
    uint32_t binding,
    VkDescriptorImageInfo* imageInfo)
{
    assert(
        m_setLayout.m_bindings.count(binding) == 1 &&
        "Layout does not contain specified binding");

    VkDescriptorSetLayoutBinding& bindingDescription =
        m_setLayout.m_bindings[binding];

    assert(
        bindingDescription.descriptorCount == 1 &&
        "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = imageInfo;
    write.descriptorCount = 1;

    m_writes.push_back(write);
    return *this;
}

bool VgeDescriptorWriter::build(VkDescriptorSet& set)
{
    bool success =
        m_pool.allocateDescriptorSet(m_setLayout.getDescriptorSetLayout(), set);
    if (!success)
    {
        return false;
    }
    overwrite(set);
    return true;
}

void VgeDescriptorWriter::overwrite(VkDescriptorSet& set)
{
    for (VkWriteDescriptorSet& write : m_writes)
    {
        write.dstSet = set;
    }
    vkUpdateDescriptorSets(
        m_pool.m_vgeDevice.device(),
        m_writes.size(),
        m_writes.data(),
        0,
        nullptr);
}
} // namespace vge

#include "vge_descriptors.hpp"

#include <cassert>
#include <stdexcept>

namespace vge {

// *************** Descriptor Set Layout Builder *********************
/* Constructs a Builder for creating a VgeDescriptorSetLayout.
 *
 * This constructor initializes the Builder with the specified VgeDevice.
 */
VgeDescriptorSetLayout::Builder::Builder(VgeDevice& vgeDevice)
    : m_vgeDevice{ vgeDevice }
{}

/* Adds a binding to the Descriptor Set Layout Builder.
 *
 * This function configures a binding for the descriptor set layout, specifying
 * the binding index, descriptor type, shader stage flags, and the number of
 * descriptors. It asserts that the binding is not already in use.
 */
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

/* Builds the VgeDescriptorSetLayout from the configured bindings.
 *
 * This function creates a unique pointer to a VgeDescriptorSetLayout instance,
 * using the bindings previously added to the Builder.
 */
std::unique_ptr<VgeDescriptorSetLayout> VgeDescriptorSetLayout::Builder::build() const
{
    return std::make_unique<VgeDescriptorSetLayout>(m_vgeDevice, m_bindings);
}

// *************** Descriptor Set Layout *********************
/* Constructs a VgeDescriptorSetLayout with specified bindings.
 *
 * This constructor initializes the descriptor set layout with the provided
 * device and bindings, and creates the Vulkan descriptor set layout.
 */
VgeDescriptorSetLayout::VgeDescriptorSetLayout(
    VgeDevice& vgeDevice,
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : m_vgeDevice{ vgeDevice }
    , m_bindings{ bindings }
{
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
    for (std::pair<const unsigned int, VkDescriptorSetLayoutBinding> kv : bindings) {
        setLayoutBindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(
            vgeDevice.getDevice(),
            &descriptorSetLayoutInfo,
            nullptr,
            &m_descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }
}

/* Destroys the VgeDescriptorSetLayout.
 *
 * This destructor cleans up the Vulkan descriptor set layout resources.
 */
VgeDescriptorSetLayout::~VgeDescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(m_vgeDevice.getDevice(), m_descriptorSetLayout, nullptr);
}

/* Retrieves the Vulkan descriptor set layout.
 *
 * This function returns the Vulkan descriptor set layout handle for use in
 * Vulkan operations.
 */
VkDescriptorSetLayout VgeDescriptorSetLayout::getDescriptorSetLayout() const
{
    return m_descriptorSetLayout;
}

// *************** Descriptor Pool Builder *********************
/* Constructs a Builder for creating a VgeDescriptorPool.
 *
 * This constructor initializes the Builder with the specified VgeDevice.
 */
VgeDescriptorPool::Builder::Builder(VgeDevice& vgeDevice)
    : m_vgeDevice{ vgeDevice }
{}

/* Adds a pool size configuration to the Descriptor Pool Builder.
 *
 * This function adds a size configuration for the descriptor pool, specifying
 * the descriptor type and the number of descriptors.
 */
VgeDescriptorPool::Builder& VgeDescriptorPool::Builder::addPoolSize(
    VkDescriptorType descriptorType,
    uint32_t count)
{
    m_poolSizes.push_back({ descriptorType, count });
    return *this;
}

/* Sets the creation flags for the Descriptor Pool Builder.
 *
 * This function allows the user to specify flags that control the creation of
 * the descriptor pool.
 */
VgeDescriptorPool::Builder& VgeDescriptorPool::Builder::setPoolFlags(
    VkDescriptorPoolCreateFlags flags)
{
    m_poolFlags = flags;
    return *this;
}

/* Sets the maximum number of descriptor sets for the Descriptor Pool Builder.
 *
 * This function specifies the maximum number of descriptor sets that can be
 * allocated from the pool.
 */
VgeDescriptorPool::Builder& VgeDescriptorPool::Builder::setMaxSets(uint32_t count)
{
    m_maxSets = count;
    return *this;
}

/* Builds the VgeDescriptorPool from the configured settings.
 *
 * This function creates a unique pointer to a VgeDescriptorPool instance,
 * using the parameters previously set in the Builder.
 */
std::unique_ptr<VgeDescriptorPool> VgeDescriptorPool::Builder::build() const
{
    return std::make_unique<VgeDescriptorPool>(m_vgeDevice, m_maxSets, m_poolFlags, m_poolSizes);
}

// *************** Descriptor Pool *********************
/* Constructs a VgeDescriptorPool with specified parameters.
 *
 * This constructor initializes the descriptor pool with the provided device,
 * maximum number of sets, creation flags, and pool sizes.
 */
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
            vgeDevice.getDevice(),
            &descriptorPoolInfo,
            nullptr,
            &m_descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
}

/* Destroys the VgeDescriptorPool.
 *
 * This destructor cleans up the Vulkan descriptor pool resources.
 */
VgeDescriptorPool::~VgeDescriptorPool()
{
    vkDestroyDescriptorPool(m_vgeDevice.getDevice(), m_descriptorPool, nullptr);
}

/* Allocates a descriptor set from the pool.
 *
 * This function attempts to allocate a descriptor set using the specified
 * descriptor set layout and returns true on success, or false if allocation
 * fails.
 */
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
    if (vkAllocateDescriptorSets(m_vgeDevice.getDevice(), &allocInfo, &descriptor) != VK_SUCCESS) {
        return false;
    }
    return true;
}

/* Frees a vector of descriptor sets back to the pool.
 *
 * This function releases the specified descriptor sets, making them available
 * for reuse in future allocations.
 */
void VgeDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
{
    vkFreeDescriptorSets(
        m_vgeDevice.getDevice(),
        m_descriptorPool,
        static_cast<uint32_t>(descriptors.size()),
        descriptors.data());
}

/* Resets the descriptor pool, freeing all allocated descriptor sets.
 *
 * This function resets the descriptor pool to its initial state, allowing
 * for reallocation of descriptor sets.
 */
void VgeDescriptorPool::resetPool()
{
    vkResetDescriptorPool(m_vgeDevice.getDevice(), m_descriptorPool, 0);
}

// *************** Descriptor Writer *********************
/* Constructs a VgeDescriptorWriter for writing to a descriptor set.
 *
 * This constructor initializes the writer with the specified descriptor set
 * layout and pool, preparing it to write descriptor information.
 */
VgeDescriptorWriter::VgeDescriptorWriter(VgeDescriptorSetLayout& setLayout, VgeDescriptorPool& pool)
    : m_setLayout{ setLayout }
    , m_pool{ pool }
{}

/* Writes buffer information to the specified binding in the descriptor set.
 *
 * This function adds a buffer info write operation for the specified binding,
 * asserting that the binding exists and expects a single descriptor.
 */
VgeDescriptorWriter& VgeDescriptorWriter::writeBuffer(
    uint32_t binding,
    VkDescriptorBufferInfo* bufferInfo)
{
    assert(
        m_setLayout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

    VkDescriptorSetLayoutBinding& bindingDescription = m_setLayout.m_bindings[binding];

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

/* Writes image information to the specified binding in the descriptor set.
 *
 * This function adds an image info write operation for the specified binding,
 * asserting that the binding exists and expects a single descriptor.
 */
VgeDescriptorWriter& VgeDescriptorWriter::writeImage(
    uint32_t binding,
    VkDescriptorImageInfo* imageInfo)
{
    assert(
        m_setLayout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

    VkDescriptorSetLayoutBinding& bindingDescription = m_setLayout.m_bindings[binding];

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

/* Builds and allocates a descriptor set using the writer.
 *
 * This function allocates a descriptor set from the pool and updates it with
 * the configured write operations. Returns true on success, or false if
 * allocation fails.
 */
bool VgeDescriptorWriter::build(VkDescriptorSet& set)
{
    bool success = m_pool.allocateDescriptorSet(m_setLayout.getDescriptorSetLayout(), set);
    if (!success) {
        return false;
    }
    overwrite(set);
    return true;
}

/* Overwrites the specified descriptor set with the configured write operations.
 *
 * This function updates the descriptor set with the write operations that have
 * been added to the writer.
 */
void VgeDescriptorWriter::overwrite(VkDescriptorSet& set)
{
    for (VkWriteDescriptorSet& write : m_writes) {
        write.dstSet = set;
    }
    vkUpdateDescriptorSets(
        m_pool.m_vgeDevice.getDevice(),
        m_writes.size(),
        m_writes.data(),
        0,
        nullptr);
}
} // namespace vge

#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {

class VgePipeline {
public:
    VgePipeline(
        VkDevice lDevice,
        const std::string& vertFilepath,
        const std::string& fragFilePath,
        VkExtent2D swapchainExtent,
        VkRenderPass renderPass);
    ~VgePipeline();
    VgePipeline(const VgePipeline&) = delete;
    VgePipeline& operator=(const VgePipeline&) = delete;

    const std::string& getVertFilePath() const;
    const std::string& getFragFilePath() const;

private:
    void createGraphicsPipeline();
    static std::vector<char> readShaderFile(const std::string& filepath);
    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

    VkDevice m_lDevice;
    const std::string& m_vertFilePath;
    const std::string& m_fragFilePath;
    VkExtent2D m_swapchainExtent;

    VkShaderModule m_vertShaderModule;
    VkShaderModule m_fragShaderModule;
    VkPipeline m_graphicsPipeline;

    // Pipeline vertex input state creation info
    VkPipelineShaderStageCreateInfo m_shaderStageCI[2];
    VkPipelineVertexInputStateCreateInfo m_vertexInputStateCI{};
    std::vector<VkVertexInputBindingDescription> m_vertexInputBindingDescriptions{};
    std::vector<VkVertexInputAttributeDescription> m_vertexInputAttributeDescriptions{};
    VkPipelineInputAssemblyStateCreateInfo m_inputAssemblyStateCI{};
    VkViewport m_viewport{};
    VkRect2D m_scissor{};
    VkPipelineViewportStateCreateInfo m_viewportStateCI{};
    VkPipelineRasterizationStateCreateInfo m_rasterizationStateCI{};
    VkPipelineMultisampleStateCreateInfo m_multisampleStateCI{};
    VkPipelineDepthStencilStateCreateInfo m_depthStencilStateCI{};
    VkPipelineColorBlendAttachmentState m_colorBlendAttachmentState{};
    VkPipelineColorBlendStateCreateInfo m_colorBlendStateCI{};
    std::vector<VkDynamicState> m_dynamicStateEnables{}; // maybe change name
    VkPipelineDynamicStateCreateInfo m_dynamicStateCI{};
    VkPipelineLayoutCreateInfo m_pipelineLayoutCI{};
    VkPipelineLayout m_pipelineLayout = nullptr;
    VkRenderPass m_renderPass = nullptr;
    uint32_t m_subpass = 0;
};

} // namespace vge

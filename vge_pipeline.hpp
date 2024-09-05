#pragma once

// headers
#include "vge_device.hpp"

// libraries
#include <vulkan/vulkan_core.h>

// std
#include <string>
#include <vector>

namespace vge
{

struct PipelineConfigInfo
{
    PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;

    PipelineConfigInfo()
        : viewportInfo{}
        , inputAssemblyInfo{}
        , rasterizationInfo{}
        , multisampleInfo{}
        , colorBlendAttachment{}
        , colorBlendInfo{}
        , depthStencilInfo{}
        , dynamicStateEnables{}
        , dynamicStateInfo{}
    {
    }
};

class VgePipeline
{
public:
    VgePipeline(
        VgeDevice& device,
        const std::string& vertFilepath,
        const std::string& fragFilePath,
        const PipelineConfigInfo& configInfo);
    ~VgePipeline();

    VgePipeline(const VgePipeline&) = delete;
    VgePipeline& operator=(const VgePipeline&) = delete;

    void bind(VkCommandBuffer commandBuffer);

    static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

private:
    static std::vector<char> readFile(const std::string& filepath);

    void createGraphicsPipeline(
        const std::string& vertFilepath,
        const std::string& fragFilePath,
        const PipelineConfigInfo& configInfo);

    void createShaderModule(
        const std::vector<char>& code,
        VkShaderModule* shaderModule);

    VgeDevice& m_vgeDevice;
    VkPipeline m_graphicsPipeline;
    VkShaderModule m_vertShaderModule;
    VkShaderModule m_fragShaderModule;
};

} // namespace vge

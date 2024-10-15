#include "vge_pipeline.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {
VgePipeline::VgePipeline(
    VkDevice lDevice,
    const std::string& vertFilepath,
    const std::string& fragFilePath,
    VkRenderPass renderPass)
    : m_lDevice{ lDevice }
    , m_vertFilePath{ vertFilepath }
    , m_fragFilePath{ fragFilePath }
    , m_renderPass{ renderPass }
{
    createGraphicsPipeline();
}

VgePipeline::~VgePipeline()
{
    vkDestroyShaderModule(m_lDevice, m_vertShaderModule, nullptr);
    vkDestroyShaderModule(m_lDevice, m_fragShaderModule, nullptr);
    vkDestroyPipeline(m_lDevice, m_graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_lDevice, m_pipelineLayout, nullptr);
}

void VgePipeline::createGraphicsPipeline()
{
    // read binary shader files
    std::vector<char> vertCode = readShaderFile(m_vertFilePath);
    std::vector<char> fragCode = readShaderFile(m_fragFilePath);

    std::cout << "Vertex Shader Code Size: " << vertCode.size() << '\n';
    std::cout << "Fragment Shader Code Size: " << fragCode.size() << '\n';

    createShaderModule(vertCode, &m_vertShaderModule);
    createShaderModule(fragCode, &m_fragShaderModule);

    // Vertex module shader stage info
    m_shaderStageCI[0] = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = m_vertShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr,
    };
    // Fragment module shader stage info
    m_shaderStageCI[1] = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = m_fragShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr,
    };

    m_vertexInputStateCI = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr,
    };

    // How to interpret and draw out vertices
    m_inputAssemblyStateCI = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    // combine viewport and scissor
    m_viewportStateCI = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = nullptr,
        .scissorCount = 1,
        .pScissors = nullptr,
    };

    // Creates pixels for our topology
    m_rasterizationStateCI = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f, // Optional
        .depthBiasClamp = 0.0f,          // Optional
        .depthBiasSlopeFactor = 0.0f,    // Optional
        .lineWidth = 1.0f,
    };

    // How rasterizer handles the edges of geometry. Basically creates
    // sub-pixels for straighter/smoother lines. These visual artefacts are
    // known as aliasing
    m_multisampleStateCI = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,          // Optional
        .pSampleMask = nullptr,            // Optional
        .alphaToCoverageEnable = VK_FALSE, // Optional
        .alphaToOneEnable = VK_FALSE,      // Optional
    };

    // Depth per fragments being rendered (closer overwrites farther)
    m_depthStencilStateCI = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {},            // Optional
        .back = {},             // Optional
        .minDepthBounds = 0.0f, // Optional
        .maxDepthBounds = 1.0f, // Optional
    };

    // How we combine colors in our framebuffer
    m_colorBlendAttachmentState = {
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,  // Optional
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
        .colorBlendOp = VK_BLEND_OP_ADD,             // Optional
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,  // Optional
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
        .alphaBlendOp = VK_BLEND_OP_ADD,             // Optional
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    // Overall color blending state for the graphics pipeline
    m_colorBlendStateCI = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY, // Optional
        .attachmentCount = 1,
        .pAttachments = &m_colorBlendAttachmentState,
        .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }, // Optional
    };

    m_dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    m_dynamicStateCI = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = static_cast<uint32_t>(m_dynamicStateEnables.size()),
        .pDynamicStates = m_dynamicStateEnables.data(),
    };

    m_pipelineLayoutCI = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 0,            // Optional
        .pSetLayouts = nullptr,         // Optional
        .pushConstantRangeCount = 0,    // Optional
        .pPushConstantRanges = nullptr, // Optional
    };

    if (vkCreatePipelineLayout(m_lDevice, &m_pipelineLayoutCI, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    // Graphics pipeline info
    VkGraphicsPipelineCreateInfo graphicsPipelineCI{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stageCount = 2,
        .pStages = m_shaderStageCI,
        .pVertexInputState = &m_vertexInputStateCI,
        .pInputAssemblyState = &m_inputAssemblyStateCI,
        .pTessellationState = nullptr,
        .pViewportState = &m_viewportStateCI,
        .pRasterizationState = &m_rasterizationStateCI,
        .pMultisampleState = &m_multisampleStateCI,
        .pDepthStencilState = &m_depthStencilStateCI,
        .pColorBlendState = &m_colorBlendStateCI,
        .pDynamicState = &m_dynamicStateCI,
        .layout = m_pipelineLayout,
        .renderPass = m_renderPass,
        .subpass = m_subpass,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    // Create graphics pipeline
    if (vkCreateGraphicsPipelines(m_lDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCI, nullptr, &m_graphicsPipeline) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create the graphics pipeline");
    }
} // namespace vge

std::vector<char> VgePipeline::readShaderFile(const std::string& filepath)
{
    // NOTE: filepath is the location of a binary file
    // ate moves file pointer to the end of the file
    // binary reads binary files
    // bitwise OR operator| combines both modes: open at end and read binary
    std::ifstream file{ filepath, std::ios::ate | std::ios::binary };

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file: " + filepath);
    }

    // since file was opened at the end, tellg() returns size of the file
    size_t fileSize = (size_t)file.tellg();
    // fileSize is the maximum characters to store
    std::vector<char> buffer(fileSize);

    file.seekg(0); // opposite of ate, go to start of file
    // Reads fileSize bytes from the file stream into the memory pointed to by
    // buffer.data()
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

void VgePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
{
    VkShaderModuleCreateInfo shaderModuleCI{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, // structType
        .pNext = nullptr,
        .flags = 0,
        .codeSize = code.size(), // size of our vector array
        .pCode = reinterpret_cast<const uint32_t*>(code.data()),
    };

    if (vkCreateShaderModule(m_lDevice, &shaderModuleCI, nullptr, shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module");
    }
}

const std::string& VgePipeline::getVertFilePath() const
{
    return m_vertFilePath;
}

const std::string& VgePipeline::getFragFilePath() const
{
    return m_fragFilePath;
}

VkPipeline VgePipeline::getGraphicsPipeline() const
{
    return m_graphicsPipeline;
}
} // namespace vge

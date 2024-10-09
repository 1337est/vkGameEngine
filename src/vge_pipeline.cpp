#include "vge_pipeline.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {
VgePipeline::VgePipeline(VkDevice lDevice, const std::string& vertFilepath, const std::string& fragFilePath)
    : m_lDevice{ lDevice }
    , m_vertFilePath{ vertFilepath }
    , m_fragFilePath{ fragFilePath }
{
    createGraphicsPipeline();
}

VgePipeline::~VgePipeline()
{
    vkDestroyShaderModule(m_lDevice, m_vertShaderModule, nullptr);
    vkDestroyShaderModule(m_lDevice, m_fragShaderModule, nullptr);
    // vkDestroyPipeline(m_lDevice, m_graphicsPipeline, nullptr);
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

    [[maybe_unused]] VkPipelineShaderStageCreateInfo shaderStages[2] = {
        // Vertex module shader stage info
        {
         .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
         .pNext = nullptr,
         .flags = 0,
         .stage = VK_SHADER_STAGE_VERTEX_BIT,
         .module = m_vertShaderModule,
         .pName = "main",
         .pSpecializationInfo = nullptr,
         },
        // Fragment module shader stage info
        {
         .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
         .pNext = nullptr,
         .flags = 0,
         .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
         .module = m_fragShaderModule,
         .pName = "main",
         .pSpecializationInfo = nullptr,
         },
    };
}

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

} // namespace vge

#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vge {

class VgePipeline {
public:
    VgePipeline(VkDevice lDevice, const std::string& vertFilepath, const std::string& fragFilePath);
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

    VkShaderModule m_vertShaderModule;
    VkShaderModule m_fragShaderModule;
};

} // namespace vge

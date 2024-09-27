#pragma once

// std
#include <string>
#include <vector>

namespace vge
{

class VgePipeline
{
public:
    VgePipeline(
        const std::string& vertFilepath,
        const std::string& fragFilePath);

private:
    static std::vector<char> readShaderFile(const std::string& filepath);

    void createGraphicsPipeline(
        const std::string& vertFilepath,
        const std::string& fragFilePath);
};

} // namespace vge

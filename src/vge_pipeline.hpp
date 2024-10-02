#pragma once
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

    const std::string& getVertFilePath() const;
    const std::string& getFragFilePath() const;

private:
    static std::vector<char> readShaderFile(const std::string& filepath);

    void createGraphicsPipeline(
        const std::string& vertFilepath,
        const std::string& fragFilePath);

    const std::string& m_vertFilePath;
    const std::string& m_fragFilePath;
};

} // namespace vge

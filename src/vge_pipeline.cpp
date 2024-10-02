#include "vge_pipeline.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace vge
{
VgePipeline::VgePipeline(
    const std::string& vertFilepath,
    const std::string& fragFilePath)
{
    std::cout << "VgePipeline constructor: Starting to create the graphics "
                 "pipeline.\n";
    std::cout << "Loading shaders from:\n  Vertex shader: " << vertFilepath
              << "\n  Fragment shader: " << fragFilePath << '\n';
    createGraphicsPipeline(vertFilepath, fragFilePath);
    std::cout
        << "VgePipeline constructor: Graphics pipeline creation complete.\n";
}

std::vector<char> VgePipeline::readShaderFile(const std::string& filepath)
{
    // NOTE: filepath is the location of a binary file
    // ate moves file pointer to the end of the file
    // binary reads binary files
    // bitwise OR operator| combines both modes: open at end and read binary
    std::ifstream file{ filepath, std::ios::ate | std::ios::binary };

    if (!file.is_open())
    {
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

void VgePipeline::createGraphicsPipeline(
    const std::string& vertFilepath,
    const std::string& fragFilePath)
{
    // read binary shader files
    std::vector<char> vertCode = readShaderFile(vertFilepath);
    std::vector<char> fragCode = readShaderFile(fragFilePath);

    std::cout << "Vertex Shader Code Size: " << vertCode.size() << '\n';
    std::cout << "Fragment Shader Code Size: " << fragCode.size() << '\n';
}

} // namespace vge

// headers
#include "vge_pipeline.hpp"

// std
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace vge
{
/* Constructs a VgePipeline object.
 *
 * Takes the file paths of the vertex and fragment shader binaries and calls
 * `createGraphicsPipeline` to set up the Vulkan graphics pipeline.
 */
VgePipeline::VgePipeline(
    const std::string& vertFilepath,
    const std::string& fragFilePath)
{

    std::cout << "Creating VgePipeline with vertex shader: " << vertFilepath
              << " and fragment shader: " << fragFilePath << std::endl;

    createGraphicsPipeline(vertFilepath, fragFilePath);
}

/* Reads the contents of a binary file into a vector of characters.
 *
 * Opens a file in binary mode, moves the file pointer to the end, reads its
 * contents into a buffer, and returns the buffer. This is primarily used to
 * read compiled shader files in Vulkan applications.
 */
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

/* Creates a graphics pipeline using the provided vertex and fragment shader
 * files.
 *
 * Reads the compiled vertex and fragment shader binaries. It serves as part of
 * the pipeline creation process for rendering in Vulkan.
 */
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

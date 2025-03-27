#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>

#include "pipeline.hpp"

static std::vector<char> read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file");
    }

    size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();

    return buffer;
}

Pipeline::Pipeline() {
    read_file("shaders/glsl/triangle/triangle.frag.spv");
    read_file("shaders/glsl/triangle/triangle.vert.spv");
}


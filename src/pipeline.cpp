#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>

#include "pipeline.hpp"
#include "device.hpp"

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

Pipeline::Pipeline(Device& dev): m_device{dev} {
    auto vert_file_bytes_v = read_file("shaders/glsl/triangle/triangle.vert.spv");
    auto frag_file_bytes_v = read_file("shaders/glsl/triangle/triangle.frag.spv");

    std::cout << "File bytes: " << vert_file_bytes_v.size() << std::endl;
    std::cout << "File bytes: " << frag_file_bytes_v.size() << std::endl;

    auto shader_module = this->create_shader_module(vert_file_bytes_v);
}

VkShaderModule Pipeline::create_shader_module(std::vector<char>& spirv_bytes) {
    /* create shader module */
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = spirv_bytes.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(spirv_bytes.data());
    
    VkShaderModule shader_module;
    if (vkCreateShaderModule(m_device.get_vk_device(), &create_info, nullptr, &shader_module)) {
        throw std::runtime_error("Failed to create shader module 😵");
    }

    return shader_module;
}


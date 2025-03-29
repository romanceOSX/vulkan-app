#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <vulkan/vulkan_core.h>

#include "device.hpp"
#include "swapchain.hpp"
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

Pipeline::Pipeline(Device& dev, SwapChain& swapchain):
    m_device{dev},
    m_swapchain{swapchain}
{
    auto vert_file_bytes_v = read_file("shaders/glsl/triangle/triangle.vert.spv");
    auto frag_file_bytes_v = read_file("shaders/glsl/triangle/triangle.frag.spv");

    std::cout << "File bytes: " << vert_file_bytes_v.size() << std::endl;
    std::cout << "File bytes: " << frag_file_bytes_v.size() << std::endl;

    auto vert_shader_module = this->create_shader_module(vert_file_bytes_v);
    auto frag_shader_module = this->create_shader_module(frag_file_bytes_v);

    /* assign vert shader module to pipeline stage */
    VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    /* entrypoint function name */
    vert_shader_stage_info.pName = "main";

    /* assign fragment shader module to pipeline stage */
    VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    vert_shader_stage_info.module = frag_shader_module;
    /* entrypoint function name */
    vert_shader_stage_info.pName = "main";

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {
        frag_shader_stage_info,
        vert_shader_stage_info,
    };

    std::vector<VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    /* dynamic state */
    VkPipelineDynamicStateCreateInfo dynamic_state {};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();
    
    /* vertex input */
    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 0;
    vertex_input_info.pVertexBindingDescriptions = nullptr;
    vertex_input_info.vertexAttributeDescriptionCount = 0;
    vertex_input_info.pVertexAttributeDescriptions = nullptr;

    /* input assembly */
    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;
    
    /* viewports and scissors */
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swapchain.get_vk_extent_2d().width);
    viewport.width = static_cast<float>(m_swapchain.get_vk_extent_2d().height);
    viewport.minDepth = 0.0f;
    viewport.minDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swapchain.get_vk_extent_2d();

    /* TODO: create a flexible config setter */
    /* dynamic state config */
    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;
    
    /* Rasterizer */
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    //rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    /* optional */
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

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


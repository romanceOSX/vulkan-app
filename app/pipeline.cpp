#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <cassert>

#include <vulkan/vulkan_core.h>

#include "utils.hpp"
#include "app_settings.hpp"

#include "device.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"
#include "vertex.hpp"

/*
 * Pipeline class
 */
Pipeline::Pipeline(Device& dev, SwapChain& swapchain):
    m_device{dev},
    m_swapchain{swapchain},
    m_render_pass{dev, swapchain}
{
    /*
     * Shader Module creation
     */
    /* TODO: move shader locations to a program options logic */
    std::cout << "printing exe directory" << std::endl;
    std::cout << utils_get_exe_dir() << std::endl;
    auto vert_shader_module = ShaderModule(m_device, utils_get_exe_dir() + "/shaders/triangle.vert.spv");
    auto frag_shader_module = ShaderModule(m_device, utils_get_exe_dir() + "/shaders/triangle.frag.spv");

    /* assign vert shader module to pipeline stage */
    VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    /* entrypoint function name */
    vert_shader_stage_info.pName = "main";

    /* assign fragment shader module to pipeline stage */
    VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = frag_shader_module;
    /* entrypoint function name */
    frag_shader_stage_info.pName = "main";

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {
        frag_shader_stage_info,
        vert_shader_stage_info,
    };

    /*
     * Vertex input bindings
     */
    /* no vertex data to load now, will be using vertex buffers instead */
    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.pNext = nullptr;
    vertex_input_info.flags = 0;
    /* vertex buffer info */
    auto binding_description = VertexInput::get_binding_description();
    auto attribute_descriptions = VertexInput::get_attribute_descriptions();
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
    vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

    /* input assembly */
    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.pNext = nullptr;
    input_assembly.flags = 0;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    //input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;
    
    /* viewports and scissors */
    /* A viewport describes the region of the framebuffer that will be rendered to */
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swapchain.get_vk_extent_2d().width);
    viewport.height = static_cast<float>(m_swapchain.get_vk_extent_2d().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swapchain.get_vk_extent_2d();

    /* TODO: create a flexible config setter */
    /* dynamic state config */
    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.pNext = nullptr;
    viewport_state.flags = 0;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &scissor;
    
    /* rasterizer */
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.pNext = nullptr;
    rasterizer.flags = 0;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    /* optional */
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    /* multisampling */
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.pNext = nullptr;
    multisampling.flags = 0;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    /* colorblending */
    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;

    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blending{};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.pNext = nullptr;
    color_blending.flags = 0;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;
    color_blending.blendConstants[0] = 0.0f;
    color_blending.blendConstants[1] = 0.0f;
    color_blending.blendConstants[2] = 0.0f;
    color_blending.blendConstants[3] = 0.0f;

    /*
     * Dynamic State
     */
    std::vector<VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state{};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.pNext = nullptr;
    dynamic_state.flags = 0;
    dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();
    
    /*
     * Pipeline Layout
     */
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.pNext = nullptr;
    pipeline_layout_info.flags = 0;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(m_device.get_vk_device(), &pipeline_layout_info, nullptr, &m_pipeline_layout)
            != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Pipeline layout 😵");
    }

    /* create graphic pipeline */
    VkGraphicsPipelineCreateInfo pipeline_info;
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.pNext = nullptr;
    pipeline_info.flags = 0;

    pipeline_info.stageCount = shader_stages.size();
    pipeline_info.pStages = shader_stages.data();

    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pDepthStencilState = nullptr;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = &dynamic_state;

    pipeline_info.layout = m_pipeline_layout;

    pipeline_info.renderPass = m_render_pass.get_vk_render_pass();
    pipeline_info.subpass = 0;

    /* FIX: pipeline/validation layer error */
    /* --> https://stackoverflow.com/questions/45460516/vkcreatepipeline-fails-and-validation-layer-segment-faults */
    pipeline_info.pTessellationState = nullptr;

    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.basePipelineIndex = -1;
                                                                                                                 
    if (vkCreateGraphicsPipelines(m_device.get_vk_device(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_vk_pipeline)
            != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Graphics Pipeline 😵");
    }
    APP_PRETTY_PRINT_CREATE("created graphics pipeline");
    APP_PRETTY_PRINT_CREATE("created pipeline layout");
    APP_PRETTY_PRINT_CREATE("created render pass");
}

RenderPass& Pipeline::get_render_pass() {
    return m_render_pass;
}

VkPipeline Pipeline::get_vk_pipeline() {
    return m_vk_pipeline;
}

Pipeline::~Pipeline() {
    vkDestroyPipeline(m_device.get_vk_device(), m_vk_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device.get_vk_device(), m_pipeline_layout, nullptr);
    vkDestroyRenderPass(m_device.get_vk_device(), m_render_pass.get_vk_render_pass(), nullptr);
    APP_PRETTY_PRINT_DESTROY("destroyed pipeline");
    APP_PRETTY_PRINT_DESTROY("destroyed pipeline layout");
    APP_PRETTY_PRINT_DESTROY("destroyed render pass");
}

/*
 * Shader module class
 * TODO: move this to its own module
 */
ShaderModule::ShaderModule(Device& dev, const std::string& filepath): m_device{dev} {
    _read_file(filepath);

    /* create shader module */
    assert((m_spirv_bytes.size() % 4) == 0);
    
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = m_spirv_bytes.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(m_spirv_bytes.data());
    
    if (vkCreateShaderModule(m_device.get_vk_device(), &create_info, nullptr, &m_vk_shader_module)
            != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module 😵");
    }
    APP_PRETTY_PRINT_CREATE("Created shader module");
}

void ShaderModule::_read_file(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file");
    }

    size_t file_size = static_cast<size_t>(file.tellg());

    m_spirv_bytes.resize(file_size);

    file.seekg(0);
    file.read(m_spirv_bytes.data(), file_size);

    file.close();
}

/* WARN: are user-defined conversion functions good practice? */
ShaderModule::operator VkShaderModule() {
    return m_vk_shader_module;
}

ShaderModule::~ShaderModule() {
    vkDestroyShaderModule(m_device.get_vk_device(), m_vk_shader_module, nullptr);
    APP_PRETTY_PRINT_DESTROY("destroyed shader module");
}


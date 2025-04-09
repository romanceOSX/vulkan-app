#pragma once

#include <vector>

#include <vulkan/vulkan_core.h>

class Device;
class SwapChain;

class RenderPass {
    public:
        RenderPass(Device& device, SwapChain& swapchain);
        VkRenderPass get_vk_render_pass();

    private:
        VkRenderPass            m_vk_render_pass;
        Device&                 m_device;
        SwapChain&              m_swapchain;
};

class Pipeline {
    public:
        Pipeline(Device& dev, SwapChain& swapchain);
        ~Pipeline();
        VkShaderModule create_shader_module(std::vector<char>& bytes);
        RenderPass& get_render_pass();
        VkPipeline get_vk_pipeline();

    private:
        RenderPass              m_render_pass;
        Device&                 m_device;
        SwapChain&              m_swapchain;
        VkPipelineLayout        m_pipeline_layout;
        VkPipeline              m_vk_pipeline;
};

/*
 * Type that encapsulates everything needed to consume a shader module
 */
class ShaderModule {
    public:
        ShaderModule(Device& dev, const std::string& filepath);
        ~ShaderModule();
        operator VkShaderModule(); /* returns the internal m_vk_shader_module */

    private:
        void  _read_file(const std::string& filename);
        std::vector<char>   m_spirv_bytes;
        Device&             m_device;
        VkShaderModule      m_vk_shader_module;
};


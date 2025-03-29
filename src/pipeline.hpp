#include <vector>

#include <vulkan/vulkan_core.h>

class Device;
class SwapChain;

class Pipeline {
    public:
        Pipeline(Device& dev, SwapChain& swapchain);
        VkShaderModule create_shader_module(std::vector<char>& bytes);

    private:
        Device&                 m_device;
        SwapChain&              m_swapchain;
        VkPipelineLayout        m_pipeline_layout;
};


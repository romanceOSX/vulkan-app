#pragma once

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


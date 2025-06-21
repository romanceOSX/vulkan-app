#pragma once

#include <vector>

#include "vulkan/vulkan.h"
#include "vulkan/vulkan_core.h"

/*
 * Forward Declarations
 */
class SwapChain;
class Pipeline;
class Device;

class Framebuffers {
    public:
        Framebuffers(Device& dev, SwapChain& swap, Pipeline& pipeline);
        ~Framebuffers();
        std::vector<VkFramebuffer>& get_swapchain_frame_buffers();

    private:
        void _create_framebuffers();

        Device&                         m_device;
        SwapChain&                      m_swapchain;
        Pipeline&                       m_pipeline;
        std::vector<VkFramebuffer>      m_swapchain_frame_buffers;
};


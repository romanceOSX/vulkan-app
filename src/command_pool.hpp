#pragma once

#include <vulkan/vulkan.h>
#include <vector>

/*
 * Forward declarations
 */
class Device;
class CommandBuffer;
class Pipeline;
class RenderPass;
class SwapChain;
class Framebuffers;

/*
 * This class is in charge of allocating a command pool for an specific device
 * at an specific queue family index
 * (command pool) <=> (device, queue family)
 *
 * After allocation it is able to allocate Command Buffers from within its internal
 * Command Pool
 */
class CommandPool {
    public:
        CommandPool() = delete;
        CommandPool(Device& dev);
        ~CommandPool();
        void trim();
        void reset();
        CommandBuffer& create_command_buffer();
        VkCommandPool get_vk_command_pool();
        std::vector<CommandBuffer>& get_command_buffers();
        Device& get_device();

    private:
        void _allocate_cmd_buffer();

        std::vector<CommandBuffer>      m_command_buffers;
        Device&                         m_device;
        VkCommandPool                   m_command_pool;
};

/* TODO: Rename this to CommandBuffers plural! */
class CommandBuffer {
    public:
        using Type = VkCommandBufferLevel;
        using Usage = VkCommandBufferUsageFlags;

        CommandBuffer(CommandPool& cmdPool, uint32_t count, Type type);
        void reset();
        void free();
        uint32_t count();
        std::vector<VkCommandBuffer>& get_command_buffers();
        /* TODO: add a prefix for command buffer related operations, this to distringuish between class management methods */
        void begin_recording();
        void cmd_begin_render_pass(uint32_t image_index, SwapChain& swapchain, RenderPass& render_pass, Framebuffers& framebuffers);
        void cmd_bind_pipeline(Pipeline& pipeline);
        void cmd_set_viewport_and_scissor(SwapChain& swapchain);
        void cmd_draw();
        void cmd_end_render_pass();
        void end_recording();
        void submit();

    private:
        Device&                         m_device;
        CommandPool&                    m_command_pool;
        std::vector<VkCommandBuffer>    m_command_buffers;
        uint32_t                        m_count;
};


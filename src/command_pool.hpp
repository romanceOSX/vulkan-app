#pragma once

#include <vulkan/vulkan.h>
#include <vector>

/*
 * Forward declarations
 */
class Device;
class CommandBuffer;
class RenderPass;
class SwapChain;
class Framebuffers;

class CommandPool {
    public:
        CommandPool() = delete;
        CommandPool(Device& dev);
        void trim();
        void reset();
        void destroy();
        CommandBuffer& create_command_buffer();
        VkCommandPool getVkCmdPool();
        Device& getDevice();

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
        std::vector<VkCommandBuffer>& getCmdBuffs();
        void begin(uint32_t image_index, SwapChain& swapchain, RenderPass& render_pass, Framebuffers& framebuffers);
        void submit();

    private:
        Device&                         m_device;
        CommandPool&                    m_command_pool;
        std::vector<VkCommandBuffer>    m_command_buffers;
        uint32_t                        m_count;
};


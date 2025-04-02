#pragma once

#include <vulkan/vulkan.h>
#include <vector>

/*
 * Forward declarations
 */
class Device;

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

        Device&                 m_device;
        VkCommandPool           m_command_pool;
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
        void begin(uint32_t buf, Usage usage);
        void submit();

    private:
        Device&                         m_device;
        CommandPool&                    m_command_pool;
        std::vector<VkCommandBuffer>    m_command_buffers;
        uint32_t                        m_count;
};


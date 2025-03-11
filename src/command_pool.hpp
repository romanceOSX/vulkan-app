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
        VkCommandPool getVkCmdPool();
        Device& getDevice();

    private:
        void _allocate_cmd_buffer();

        Device&                 _m_dev;
        VkCommandPool           _m_command_pool;
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
        Device&                         _m_dev;
        CommandPool&                    _m_cmdPool;
        std::vector<VkCommandBuffer>    _m_cmdBuffs;
        uint32_t                        _m_count;
};


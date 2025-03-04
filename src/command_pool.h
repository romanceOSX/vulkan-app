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

    private:
        void _allocate_cmd_buffer();

        Device&                 _m_dev;
        VkCommandPool           _m_command_pool;
};


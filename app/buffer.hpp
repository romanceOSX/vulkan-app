#pragma once

#include <vulkan/vulkan.h>

class Device;
class CommandPool;

class Buffer {
    public:
        static void copy_buffer(CommandPool& cmd_pool, Buffer& src, Buffer& dst, VkDeviceSize size);

        Buffer(Device& dev, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
        VkBuffer get_vk_buf();
        Device& get_dev();
        void map_memory(void** data);
        void unmap_memory();

    private:
        Device&             m_dev;
        VkBuffer            m_vk_buffer;
        VkDeviceMemory      m_vk_buffer_memory;
        VkDeviceSize        m_size;
};


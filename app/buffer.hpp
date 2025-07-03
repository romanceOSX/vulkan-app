#pragma once

#include <vulkan/vulkan.h>

class Device;

class Buffer {
    public:
        Buffer(Device& dev, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

    private:
        Device&             m_dev;
        VkBuffer            m_vk_buffer;
        VkDeviceMemory      m_vk_buffer_memory;
};


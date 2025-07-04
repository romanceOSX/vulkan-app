#include <vulkan/vulkan.h>

#include <optional>

#include "app_settings.hpp"
#include "buffer.hpp"
#include "device.hpp"
#include "physical_device.hpp"

Buffer::Buffer(
    Device& dev,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties
): 
    m_dev{dev},
    m_size{size}
{
    auto phy_dev = m_dev.get_physical_device();

    /* create buffer */
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.pNext = nullptr;
    buffer_info.size = m_size;
    /* VkBufferCreateInfo::usage specifies the usage of the memory being allocated */
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffer_info.flags = 0;

    if (vkCreateBuffer(m_dev, &buffer_info, nullptr, &m_vk_buffer)
            != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vertex buffer 😵");
    }
    APP_PRETTY_PRINT_CREATE("Created buffer");

    /* query buffer's memory requirements */
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(m_dev, m_vk_buffer, &mem_requirements);

    /* allocate device memory */
    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = phy_dev.find_memory_properties(
        mem_requirements.memoryTypeBits, 
        properties
    ).value();

    if (vkAllocateMemory(m_dev, &alloc_info, nullptr, &m_vk_buffer_memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory");
    }
    APP_PRETTY_PRINT_CREATE("allocated vertex buffer memory");

    /* associate allocated device memory with the buffer */
    vkBindBufferMemory(m_dev, m_vk_buffer, m_vk_buffer_memory, 0);
}

void Buffer::map_memory(void** data) {
    vkMapMemory(m_dev, m_vk_buffer_memory, 0, m_size, 0, &(*data));
}

void Buffer::unmap_memory() {
    vkUnmapMemory(m_dev, m_vk_buffer_memory);
}

VkBuffer Buffer::get_vk_buffer() {
    return m_vk_buffer;
}


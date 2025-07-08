#include <vulkan/vulkan.h>

#include <optional>
#include <vulkan/vulkan_core.h>

#include "app_settings.hpp"
#include "buffer.hpp"
#include "command_pool.hpp"
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

VkBuffer Buffer::get_vk_buf() {
    return m_vk_buffer;
}

Device& Buffer::get_dev() {
    return m_dev;
}

void Buffer::copy_buffer(
    CommandPool& cmd_pool,
    Buffer &src,
    Buffer &dst,
    VkDeviceSize size
) {
    auto& dev = src.get_dev();

    /* create temporary command buffer to command memory transfer operation */
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = cmd_pool.get_vk_command_pool();
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer cmd_buf;
    vkAllocateCommandBuffers(dev.get_vk_device(), &alloc_info, &cmd_buf);

    /* record operation */
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd_buf, &begin_info);

    /* copy the memory from src to dst */
    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = size;
    vkCmdCopyBuffer(cmd_buf, src.get_vk_buf(), dst.get_vk_buf(), 1, &copy_region);

    vkEndCommandBuffer(cmd_buf);

    /* submit the command */
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buf;

    /* get queue out of device */ 
    VkQueue queue = cmd_pool.get_device().get_vk_queue(cmd_pool.get_queue_family());

    /* we could wait through a fence but it is not necesary */
    vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(dev.get_vk_device(), cmd_pool.get_vk_command_pool(), 1, &cmd_buf);
}


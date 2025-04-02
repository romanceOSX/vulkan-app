#include <stdexcept>
#include <tuple>

#include <vulkan/vulkan_core.h>

#include "app_settings.hpp"
#include "command_pool.hpp"
#include "device.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"
#include "framebuffers.hpp"

/*
 * Command Pool class
 */
CommandPool::CommandPool(Device& dev): m_device{dev} {
    APP_PRETTY_PRINT_CUSTOM("creating command pool...", "☀️");
    VkCommandPool commandPool;
    VkCommandPoolCreateInfo command_pool_create = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = m_device.get_queue_family_index(),
    };

    if (VK_SUCCESS != vkCreateCommandPool(
                dev.get_vk_device(), &command_pool_create,
                nullptr, &commandPool)) {
        APP_DBG_ERR("Failed to create Command Pool");
    }
}

void CommandPool::reset() {
    vkResetCommandPool(
            m_device.get_vk_device(),
            m_command_pool,
            VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
    );
}

void CommandPool::destroy() {
    vkDestroyCommandPool( m_device.get_vk_device(),
            m_command_pool,
            nullptr
    );
}

VkCommandPool CommandPool::getVkCmdPool() {
    return m_command_pool;
}

Device& CommandPool::getDevice() {
    return m_device;
}

CommandBuffer& CommandPool::create_command_buffer() {
    m_command_buffers.emplace_back(*this, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY); 
    return m_command_buffers.back();
}

/*
 * TODO: Find a better way to pass the Device around 
 *  all classes in some way related to device, should have a reference to it
 *  as a member
 */

/*
 * Command Buffer class
 */
CommandBuffer::CommandBuffer(CommandPool& cmdPool, uint32_t count, Type type)
    :m_command_pool{cmdPool},
    m_count{count},
    m_device{cmdPool.getDevice()}
{
    /* TODO: This vector could be initialized at the initializer list instead */
    m_command_buffers.resize(m_count);

    VkCommandBufferAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = m_command_pool.getVkCmdPool(),
        .level = type,
        .commandBufferCount = m_count,
    };

    vkAllocateCommandBuffers(
            m_device.get_vk_device(),
            &allocate_info,
            m_command_buffers.data()
    );  
}

/* Resets all, edit class upon solid use-case */
void CommandBuffer::reset() {
    for (auto& buf: m_command_buffers) {
        vkResetCommandBuffer(buf, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    }
}

std::vector<VkCommandBuffer>& CommandBuffer::getCmdBuffs() {
    return m_command_buffers;
}

void CommandBuffer::free() {
    vkFreeCommandBuffers(m_device.get_vk_device(),
            m_command_pool.getVkCmdPool(),
            m_command_buffers.size(),
            m_command_buffers.data()
    );
}

void CommandBuffer::begin(uint32_t buf, Usage usage) {
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = usage,
        .pInheritanceInfo = nullptr,
    };
    vkBeginCommandBuffer(m_command_buffers.at(buf), &beginInfo);
}

/*
 * Command Buffer submission
 */
void CommandBuffer::submit() {
    auto tup = std::make_tuple(1, "hello");
    auto [a, b] = tup;
}


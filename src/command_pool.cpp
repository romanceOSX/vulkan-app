
#include "command_pool.hpp"
#include "host.hpp"
#include "device.hpp"

#include <vulkan/vulkan_core.h>

/*
 * Command Pool class
 */
CommandPool::CommandPool(Device& dev): _m_dev{dev} {
    VkCommandPool commandPool;
    VkCommandPoolCreateInfo command_pool_create = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = _m_dev.getQueueFamilyIndex(),
    };

    if (VK_SUCCESS != vkCreateCommandPool(
                dev.getVkDevice(), &command_pool_create,
                nullptr, &commandPool)) {
        DBG_ERR("Failed to create Command Pool");
    }
}

void CommandPool::reset() {
    vkResetCommandPool(
            _m_dev.getVkDevice(),
            _m_command_pool,
            VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
    );
}

void CommandPool::destroy() {
    vkDestroyCommandPool(_m_dev.getVkDevice(),
            _m_command_pool,
            nullptr
    );
}

VkCommandPool CommandPool::getVkCmdPool() {
    return _m_command_pool;
}

Device& CommandPool::getDevice() {
    return _m_dev;
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
    :_m_cmdPool{cmdPool}, _m_count{count}, _m_dev{cmdPool.getDevice()} {
    /* TODO: This vector could be initialized at the initializer list instead */
    _m_cmdBuffs.resize(_m_count);
    
    VkCommandBufferAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = _m_cmdPool.getVkCmdPool(),
        .level = type,
        .commandBufferCount = _m_count,
    };

    vkAllocateCommandBuffers(
            _m_dev.getVkDevice(),
            &allocate_info,
            _m_cmdBuffs.data()
    );  
}

/* Resets all, edit class upon solid use-case */
void CommandBuffer::reset() {
    for (auto& buf: _m_cmdBuffs) {
        vkResetCommandBuffer(buf, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    }
}

std::vector<VkCommandBuffer>& CommandBuffer::getCmdBuffs() {
    return _m_cmdBuffs;
}

void CommandBuffer::free() {
    vkFreeCommandBuffers(_m_dev.getVkDevice(),
            _m_cmdPool.getVkCmdPool(),
            _m_cmdBuffs.size(),
            _m_cmdBuffs.data()
    );
}

void CommandBuffer::begin(uint32_t buf, Usage usage) {
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = usage,
        .pInheritanceInfo = nullptr,
    };
    vkBeginCommandBuffer(_m_cmdBuffs.at(buf), &beginInfo);
}

#include <tuple>
/*
 * Command Buffer submission
 */
void CommandBuffer::submit() {
    auto tup = std::make_tuple(1, "hello");
    auto [a, b] = tup;
}


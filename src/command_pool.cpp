#include "command_pool.h"

#include "host.h"
#include "device.h"
#include <vulkan/vulkan_core.h>

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

void CommandPool::_allocate_cmd_buffer() {
    /* Allocate command buffer from cmd buff pool */
    VkCommandBuffer cmdBuff;

    VkCommandBufferAllocateInfo allocate_info;

    vkAllocateCommandBuffers(
            _m_dev.getVkDevice(),
            &allocate_info,
            &cmdBuff
    );  
}


#include "device.hpp"
#include "synchronization.hpp"

VkSemaphore Semaphore::create(Device& dev) {
    VkSemaphore semaphore;
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_info.pNext = nullptr;
    semaphore_info.flags = 0;

    /* FIX: semaphores also need to be destroyed */
    if (vkCreateSemaphore(dev.get_vk_device(), &semaphore_info, nullptr, &semaphore)
            != VK_SUCCESS) 
    {
        throw std::runtime_error("Semaphore creation failed! 😵");
    }
    return semaphore;
}

VkFence Fence::create(Device& dev) {
    VkFence fence;
    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.pNext = nullptr;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(dev.get_vk_device(), &fence_info, nullptr, &fence)
            != VK_SUCCESS) 
    {
        throw std::runtime_error("Fence creation failed! 😵");
    }
    return fence;
}


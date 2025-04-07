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


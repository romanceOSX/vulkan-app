#include "vulkan/vulkan_core.h"

class Device;

class Semaphore {
    public:
        static VkSemaphore create(Device& dev);
};

class Fence {
    public:
        static VkFence create(Device& dev);
};


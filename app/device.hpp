#pragma once

#include <vector>

#include "vulkan/vulkan.h"
#include <vulkan/vulkan_core.h>

/*
 * Forward declarations
 */
class PhysicalDevice;
class QueueFamily;
class Window;
class Device;
enum class AppResult;

/*
 * device
 *  -> physical device
 *      -> window
 *  -> window
 */

/*
 * NOTE: information about queue families is externally managed, meaning that the
 *       class alone won't be able to tell which queue is used for windowing, computing
 *       or any other task
 */

/* TODO: Implement logical device wrapper, see Instance class for reference */
/* TODO: Add validation for existing, valid extensions */
/*
 * Class responsible for:
 *  - Querying host's availbale device(s) information
 *  - Creating logical devices
 *  - Querying device's properties (queues)
 */
class Device {
    public:
        Device() = delete;
        Device(Device& other) = delete;
        Device(PhysicalDevice& dev);
        ~Device();
        operator VkDevice();
        void wait(void);
        void add_extension(const char*);
        void add_queue(const QueueFamily& queue_family, uint32_t count, float priority);
        void init();
        void insert_queue();
        VkQueue get_vk_queue(const QueueFamily& queue_family);
        uint32_t get_queue_family_index();
        VkDevice get_vk_device();
        PhysicalDevice& get_physical_device();
        VkPhysicalDevice get_vk_physical_dev();
        void print_info();

    private:
        PhysicalDevice&                         m_physical_device;
        VkDevice                                m_vk_device;
        std::vector<const char*>                m_extensions;
        std::vector<VkDeviceQueueCreateInfo>    m_dev_queue_create_infos;
        bool                                    m_is_init = false;
};

/*
 * Create devices
 * Create queues through their indices? or through their queuefamily structure?
 * add_queue(flags, count, priority);
 * add_queue(QueueFamily, count, priority)
 */


#pragma once

#include <vector>

#include "vulkan/vulkan.h"
#include <vulkan/vulkan_core.h>

class Instance;

// TODO: Implement logical device wrapper, see Instance class for reference
/*
 * Class responsible for:
 *  - Querying host's availbale device(s) information
 *  - Creating logical devices
 *  - Querying device's properties (queues)
 */
class Device {
    public:
        void getDeviceQueueProperties(VkPhysicalDevice& dev);
        void getPhysicalDeviceGroups(void);

        VkDevice                                m_device;
        VkInstance                            m_instance;
        VkPhysicalDevice*                          m_gpu;
        VkPhysicalDeviceProperties           m_gpu_props;
        VkPhysicalDeviceMemoryProperties m_gpu_mem_props;
        std::vector<VkExtensionProperties>  m_extensions;
        std::vector<VkQueueFamilyProperties>    m_queue_family_props;
        uint32_t    m_graphics_queue_family_index;
        uint32_t    m_queue_family_count;
        Device() = delete;
        Device(VkInstance instance): m_instance{instance} { }

    private:
        uint32_t _get_default_index(void);
        VkPhysicalDevice _get_default_device(void);
};


#pragma once

#include <string>
#include "vulkan/vulkan.h"
#include "vector"
#include <vulkan/vulkan_core.h>
#include "instance.h"

/*
 * Class responsible for:
 *  - Querying host's availbale device(s) information
 *  - Creating logical devices
 *  - Querying device's properties (queues)
 */
class Device {
    public:
        VkDevice getDefaultDevice();
        void getDeviceExtensions(VkPhysicalDevice* dev, Instance* instance);
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


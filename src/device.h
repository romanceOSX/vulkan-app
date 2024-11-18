#pragma once

#include <vector>

#include "vulkan/vulkan.h"
#include <vulkan/vulkan_core.h>

class Instance;

// TODO: make constructor private and only accessible to internal classes
// TODO: query layer-specific extensions?
/* 
 * Representation of a single Physical Device
 * Gets physical device properties:
 *  - queues
 *  - type of device
 */
class PhysicalDevice {
    friend class App;
    friend class Instance;

    public:
        PhysicalDevice(VkPhysicalDevice dev);
        PhysicalDevice() = delete;
        VkPhysicalDeviceProperties& getDeviceProperties(void);
        uint32_t getQueueCount(void);
        std::vector<VkQueueFamilyProperties>& getDeviceQueueProperties(void);
        std::vector<VkExtensionProperties>& getDeviceExtensions(void);

    private:
        void _queryDeviceProperties();
        void _queryDeviceExtensions();
        void _queryQueueProperties();

        VkPhysicalDeviceProperties                  _m_physical_dev_props;
        VkPhysicalDevice                            _m_physical_dev;
        std::vector<VkExtensionProperties>          _m_available_extensions;
        std::vector<VkQueueFamilyProperties>        _m_available_queues;
        uint32_t                                    _m_queueCount;
};

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


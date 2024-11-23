#pragma once

#include <vector>

#include "vulkan/vulkan.h"
#include <vulkan/vulkan_core.h>


// TODO: make constructor private and only accessible to internal classes
// TODO: query layer-specific extensions?
/* 
 * Representation of a single Physical Device
 * Gets physical device properties:
 *  - queues
 *  - type of device
 */
class PhysicalDevice {
    friend class Host;
    friend class Instance;

    public:
        PhysicalDevice(VkPhysicalDevice dev);
        PhysicalDevice() = delete;
        VkPhysicalDeviceProperties& getDeviceProperties(void);
        VkPhysicalDevice getVkPhysicalDevice(void);
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


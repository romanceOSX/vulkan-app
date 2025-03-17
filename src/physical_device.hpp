#pragma once

#include <cstdint>
#include <vector>

#include "vulkan/vulkan.h"
#include <vulkan/vulkan_core.h>

class Window;
class QueueFamily;

/*
 * Note that to query physical properties of devices you don't actually need
 * a VkInstance
 */

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
        PhysicalDevice() = delete;
        PhysicalDevice(VkPhysicalDevice dev);
        std::optional<uint32_t> get_suitable_queue_index(Window& window);
        VkPhysicalDeviceProperties& getDeviceProperties(void);
        VkPhysicalDevice get_vk_physical_device(void);
        uint32_t getQueueCount(void);
        std::vector<VkQueueFamilyProperties>& getDeviceQueueProperties(void);
        std::vector<VkExtensionProperties>& getDeviceExtensions(void);
        void print_info();

    private:
        void _queryDeviceProperties();
        void _queryDeviceExtensions();
        void _queryQueueProperties();

        VkPhysicalDeviceProperties                  m_vk_physical_device_props;
        VkPhysicalDevice                            m_vk_physical_device;
        std::vector<VkExtensionProperties>          m_available_extensions;
        std::vector<VkQueueFamilyProperties>        m_vk_queue_families;
        uint32_t                                    m_queue_family_count;
};

/*
 * Class that represents a single queue family, mainly used for determining
 * correct requirements for graphic and computing capabilities
 */
class QueueFamily {
    public:
        QueueFamily(VkQueueFamilyProperties& queue_family,
                uint32_t family_index,
                PhysicalDevice& phy_dev,
                Window& window);
        bool is_graphics();
        bool is_presentation();
        uint32_t get_index();

    private:
        VkQueueFamilyProperties&        m_vk_queue_family;
        uint32_t                        m_queue_family_index;
        Window&                         m_window;
        PhysicalDevice&                 m_physical_device;
        bool                            m_is_graphics       = false;
        bool                            m_is_presentation   = false;
        std::optional<uint32_t>         m_suitable_queue_family_index;
};



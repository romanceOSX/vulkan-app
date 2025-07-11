#pragma once

#include <cstdint>
#include <vector>
#include <optional>

#include "vulkan/vulkan.h"
#include <vulkan/vulkan_core.h>

class Window;
class QueueFamily;
class SwapchainSupportDetails;

/*
 * Note that to query physical properties of devices you don't actually need
 * a VkInstance
 */

// TODO: make constructor private and only accessible to internal classes
// TODO: query layer-specific extensions?

/* Swapchain details helper structure */
struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> preset_modes;
};

/* 
 * TODO: update documentation of this class
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
        std::optional<QueueFamily> is_window_surface_compatible(Window& window);
        VkPhysicalDeviceProperties& get_vk_physical_device_properties(void);
        VkPhysicalDevice get_vk_physical_device(void);
        uint32_t get_queue_families_count(void);
        std::vector<VkQueueFamilyProperties>& get_vk_device_queue_families_properties(void);
        std::vector<VkExtensionProperties>& get_vk_physical_device_extensions(void);
        VkPhysicalDeviceMemoryProperties get_vk_physical_device_memory_properties(void);
        std::optional<uint32_t> find_memory_properties(uint32_t type_mask, VkMemoryPropertyFlags properties);
        std::optional<QueueFamily> find_queue_family(VkQueueFlagBits flag);
        
        /* TODO: rename this to 'surface_support_details' */
        SwapchainSupportDetails& get_swapchain_support_details();
        void print_info();

    private:
        void _query_physical_device_properties();
        void _query_physical_device_extensions();
        void _query_queue_families();
        void _query_physical_device_memory_properties();
        QueueFamily _pop_queue_family(QueueFamily& queue);

        VkPhysicalDeviceProperties                  m_vk_physical_device_properties;
        VkPhysicalDevice                            m_vk_physical_device;
        VkPhysicalDeviceMemoryProperties            m_vk_physical_device_memory_properties;
        std::vector<VkExtensionProperties>          m_vk_physical_device_extensions;
        std::vector<VkQueueFamilyProperties>        m_vk_queue_families;
        std::vector<QueueFamily>                    m_queue_families;
        uint32_t                                    m_queue_families_count;
        SwapchainSupportDetails                     m_swapchain_support_details;
};

/*
 * Class that represents a single queue family, mainly used for determining
 * correct requirements for graphic and computing capabilities
 */
/* TODO: make the print_info a class trait */
/* TODO: make print functions const */
class QueueFamily {
    public:
        QueueFamily(VkPhysicalDevice physical_device, VkQueueFamilyProperties queue_family, uint32_t index);
        bool is_window_supported(Window& window);
        bool is_flag_supported(VkQueueFlagBits flag);
        uint32_t count() const;
        uint32_t get_index() const;
        /* TODO: print info should return a string, not directly to stdout */
        void print_info();
        /* TODO: add '==' is_equal comparator by comparing the queue family index */

    private:
        VkPhysicalDevice            m_vk_physical_device;
        VkQueueFamilyProperties     m_vk_queue_family_properties;
        uint32_t                    m_queue_family_index;
        bool                        m_is_graphics       = false;
        bool                        m_is_presentation   = false;
        bool                        m_is_presentation_queried = false;
        std::optional<uint32_t>     m_suitable_queue_family_index;
};


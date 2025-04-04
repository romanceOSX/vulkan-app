#pragma once

#include <cstdint>
#include <vector>

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
        std::optional<uint32_t> get_suitable_queue_family_index(Window& window);
        VkPhysicalDeviceProperties& get_vk_physical_device_properties(void);
        VkPhysicalDevice get_vk_physical_device(void);
        uint32_t get_queue_families_count(void);
        std::vector<VkQueueFamilyProperties>& get_vk_device_queue_families_properties(void);
        std::vector<VkExtensionProperties>& get_vk_physical_device_extensions(void);
        SwapchainSupportDetails& get_swapchain_support_details();
        void print_info();

    private:
        void _query_physical_device_properties();
        void _query_physical_device_extensions();
        void _query_queue_families();

        VkPhysicalDeviceProperties                  m_vk_physical_device_properties;
        VkPhysicalDevice                            m_vk_physical_device;
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
        QueueFamily(VkPhysicalDevice physical_device, VkQueueFamilyProperties& queue_family, uint32_t index);
        void query_surface_support(Window& window);
        bool is_graphics();
        bool is_presentation();
        uint32_t get_index();
        void print_info();

    private:
        VkPhysicalDevice                m_vk_physical_device;
        VkQueueFamilyProperties&        m_vk_queue_family_properties;
        uint32_t                        m_queue_family_index;
        bool                            m_is_graphics       = false;
        bool                            m_is_presentation   = false;
        bool                            m_is_presentation_queried = false;
        std::optional<uint32_t>         m_suitable_queue_family_index;
};


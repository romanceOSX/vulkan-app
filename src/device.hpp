#pragma once

#include <vector>

#include "vulkan/vulkan.h"
#include <vulkan/vulkan_core.h>

/*
 * Forward declarations
 */
class PhysicalDevice;
class Window;
enum class AppResult;

/*
 * device
 *  -> physical device
 *      -> window
 *  -> window
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
        Device(PhysicalDevice& dev, Window& window): m_physical_device{dev}, m_window{window} { }
        ~Device();
        void wait(void);
        void addExtension(const char*);
        AppResult init(uint32_t count);
        VkQueue get_vk_queue();
        uint32_t getQueueFamilyIndex();
        VkDevice getVkDevice();
        VkPhysicalDevice get_vk_physical_dev();

    private:
        uint32_t _get_suitable_queue_index(void);

        PhysicalDevice&                         m_physical_device;
        Window&                                 m_window;
        VkDevice                                m_vk_device;
        VkQueue                                 m_vk_queue;
        VkPhysicalDeviceProperties              m_gpu_props;
        VkPhysicalDeviceMemoryProperties        m_gpu_mem_props;
        std::vector<const char*>                m_extensions;
        std::vector<VkQueueFamilyProperties>    m_queue_family_props;
        uint32_t                                m_graphics_queue_family_index;
        uint32_t                                m_queue_family_count;
        uint32_t                                m_queue_family_index;
};

class SwapChain {
    public:
        SwapChain(const VkPhysicalDevice phy_dev, Window& window);
        void print_info();

    private:
        VkPhysicalDevice                        m_vk_phy_dev;
        Window&                                 m_window;
        VkSurfaceCapabilitiesKHR                m_vk_surface_capabilities;
};


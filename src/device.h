#pragma once

#include <vector>

#include "vulkan/vulkan.h"
#include <vulkan/vulkan_core.h>

/*
 * Forward declarations
 */
class PhysicalDevice;
enum class AppResult;

// TODO: Implement logical device wrapper, see Instance class for reference
// TODO: Add validation for existing, valid extensions
/*
 * Class responsible for:
 *  - Querying host's availbale device(s) information
 *  - Creating logical devices
 *  - Querying device's properties (queues)
 */
class Device {
    public:
        Device() = delete;
        Device(PhysicalDevice& dev): _m_app_physical_dev{dev} { }
        void wait(void);
        AppResult addExtension(const char*);
        AppResult init(void);
        void getDeviceQueueProperties(VkPhysicalDevice& dev);
        VkQueue getDeviceQueue();
        uint32_t getQueueFamilyIndex();
        VkDevice getVkDevice();

    private:
        uint32_t _get_default_index(void);
        VkPhysicalDevice _get_default_device(void);

        PhysicalDevice&                         _m_app_physical_dev;
        VkDevice                                _m_device;
        VkQueue                                 _m_queue;
        VkPhysicalDeviceProperties              _m_gpu_props;
        VkPhysicalDeviceMemoryProperties        _m_gpu_mem_props;
        std::vector<const char*>                _m_extensions;
        std::vector<VkQueueFamilyProperties>    _m_queue_family_props;
        uint32_t                                _m_graphics_queue_family_index;
        uint32_t                                _m_queue_family_count;
        uint32_t                                _m_queue_family_index;
};


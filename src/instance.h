#pragma once

#include <string>
#include <vector>

#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan.h"

/*
 * Forward Declarations
 */
enum class AppResult;

struct LayerProperties {
    VkLayerProperties                       properties;
    std::vector<VkExtensionProperties>      extensions;
};

class Instance {
    using char_v = std::vector<const char*>;

    public:
        Instance();
        VkResult init(void);
        VkInstance get_vk_instance(void);
        std::vector<VkPhysicalDevice>& get_vk_devices(void);
        Instance(Instance& other) = delete;
        AppResult addExtension(const char* ext);
        AppResult addLayer(const char* layer);
        bool isInit(void);

    private:
        uint32_t                                _m_api_version;
        std::vector<LayerProperties>            _m_available_layers;
        std::vector<VkExtensionProperties>      _m_available_instance_extensions;
        std::vector<VkPhysicalDevice>           m_vk_physical_devices;
        VkInstance                              m_vk_instance;
        std::vector<const char*>                _m_extensions;
        std::vector<const char*>                _m_layers;
        bool                                    _m_is_init = false;
};


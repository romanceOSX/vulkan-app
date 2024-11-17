#pragma once

#include <string>
#include "vulkan/vulkan.h"
#include "vector"
#include <vulkan/vulkan_core.h>

struct LayerProperties {
    VkLayerProperties                       properties;
    std::vector<VkExtensionProperties>      extensions;
};

class Instance {
    using char_v = std::vector<const char*>;
    public:
        Instance() = default;
        VkResult init(void);
        VkInstance getInstance(void);
        VkResult createInstance(char_v& layers, char_v& extensions);
        Instance(Instance& other) = delete;
        VkResult getInstanceLayerProperties();
        uint32_t getInstanceVersion();
        std::vector<LayerProperties> m_layers;
        std::vector<VkExtensionProperties> m_instance_extensions;
        VkResult print_info();
        VkInstance m_instance;
    private:
        uint32_t m_api_version;
};


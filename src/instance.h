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
        VkInstance getInstance(void);
        VkResult createInstance(char_v& layers, char_v& extensions);
        Instance(Instance& other) = delete;
        AppResult addExtension(const char* ext);
        AppResult addLayer(const char* layer);

        std::vector<LayerProperties> m_layers;
        std::vector<VkExtensionProperties> m_instance_extensions;
        VkInstance m_instance;

    private:
        uint32_t m_api_version;
};


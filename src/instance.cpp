#include <iostream>
#include <string>
#include "instance.h"
#include "app.h"

#define VK_USE_PLATFORM_MACOS_MVK
#include "vulkan/vulkan.h"
#include <vulkan/vulkan_core.h>

Instance::Instance() {
}

VkResult Instance::createInstance(char_v& layers, char_v& extensions) {
    VkInstanceCreateInfo instance_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
        .pApplicationInfo = nullptr,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
    };

    instance_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instance_info.ppEnabledExtensionNames = extensions.data();

    instance_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
    instance_info.ppEnabledLayerNames = layers.data();

    VkResult result = vkCreateInstance(&instance_info, nullptr, &m_instance);

    if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
        DBG_ERR("Incompatible Driver!!!");
            return result;
    }
    else if (result != VK_SUCCESS) {
        DBG_ERR("Unknown Error");
            return result;
    }

    return result;
}

VkInstance Instance::getInstance() {
    return m_instance;
}

VkResult Instance::init() {
    VkResult res;
    std::vector<const char*> extensions {
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
    };

    std::vector<const char*> layers {
        "VK_LAYER_LUNARG_api_dump",
    };

    res = createInstance(layers, extensions);

    return res;
}


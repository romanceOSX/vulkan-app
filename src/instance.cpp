#include <iostream>
#include <string>

#define VK_USE_PLATFORM_MACOS_MVK
#include "vulkan/vulkan.h"
#include <vulkan/vulkan_core.h>

#include "instance.h"
#include "app.h"

Instance::Instance() { }

AppResult Instance::addExtension(const char* ext) {
    AppResult res = AppResult::APP_SUCCESS;

    _m_extensions.push_back(ext);

    return res;
}

AppResult Instance::addLayer(const char* ext) {
    AppResult res = AppResult::APP_SUCCESS;

    _m_layers.push_back(ext);

    return res;
}

VkResult Instance::init() {
    VkInstanceCreateInfo instance_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
        .pApplicationInfo = nullptr,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
    };

    instance_info.enabledExtensionCount = static_cast<uint32_t>(_m_extensions.size());
    instance_info.ppEnabledExtensionNames = _m_extensions.data();

    instance_info.enabledLayerCount = static_cast<uint32_t>(_m_layers.size());
    instance_info.ppEnabledLayerNames = _m_layers.data();

    VkResult result = vkCreateInstance(&instance_info, nullptr, &_m_instance);

    if (result == VK_SUCCESS) {
        _m_is_init = true;
    }

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
    return _m_instance;
}


bool Instance::isInit(void) {
    return _m_is_init;
}


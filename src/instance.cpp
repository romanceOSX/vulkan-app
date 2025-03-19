#include <iostream>
#include <string>

#define VK_USE_PLATFORM_MACOS_MVK
#include "vulkan/vulkan.h"
#include <vulkan/vulkan_core.h>

#include "app_settings.hpp"
#include "instance.hpp"
#include "host.hpp"

Instance::Instance() { }

AppResult Instance::add_extension(const char* ext) {
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

    VkResult result = vkCreateInstance(&instance_info, nullptr, &m_vk_instance);

    if (result == VK_SUCCESS) {
        _m_is_init = true;
    }
    if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
        APP_DBG_ERR("Incompatible Driver!!!");
            return result;
    }
    else if (result != VK_SUCCESS) {
        APP_DBG_ERR("Unknown Error");
            return result;
    }

    /* Query physical devices available */
    _query_physical_devices();

    return result;
}

VkInstance Instance::get_vk_instance() {
    return m_vk_instance;
}


bool Instance::isInit(void) {
    return _m_is_init;
}

void Instance::_query_physical_devices() {
    uint32_t devCount;

    vkEnumeratePhysicalDevices(m_vk_instance, &devCount, nullptr);
    m_vk_physical_devices.resize(devCount);
    vkEnumeratePhysicalDevices(m_vk_instance, &devCount, m_vk_physical_devices.data());

    /* TODO: Query physical device groups as well */
    /* get physical device groups */
    //uint32_t devGroupsCount;

    //vkEnumeratePhysicalDeviceGroups(m_vk_instance, &devGroupsCount, nullptr);
    //_m_physical_dev_groups.resize(devGroupsCount);
    //vkEnumeratePhysicalDeviceGroups(m_vk_instance, &devGroupsCount, _m_physical_dev_groups.data());
}

std::vector<VkPhysicalDevice>& Instance::get_vk_devices() {
    return m_vk_physical_devices;
}

Instance::~Instance() {
    vkDestroyInstance(m_vk_instance, nullptr);
}


#include <iostream>
#include <cassert>

#include "vulkan/vulkan_core.h"

#include "host.h"
#include "device.h"
#include "physical_device.h"

void Device::getDeviceQueueProperties(VkPhysicalDevice& dev) {
    uint32_t propCount;
    std::vector<VkQueueFamilyProperties> props;

    vkGetPhysicalDeviceQueueFamilyProperties(dev, &propCount, nullptr);
    props.resize(propCount);
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &propCount, props.data());

    /* queue properties */
    PRETTY_PRINT("Physical device queue properties");
    for (auto& prop: props) {
        std::cout << "Queue Count: " << prop.queueCount << std::endl
            << "Queue Flags: " << prop.queueFlags << std::endl;
    }

    /* queue Creation */
    VkDeviceQueueCreateInfo queue_create_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = _get_default_index(),
    };

    VkDeviceCreateInfo devCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    VkDevice device;

    if (VK_SUCCESS != vkCreateDevice(dev, &devCreateInfo, nullptr, &device)) {
        DBG_ERR("NOT SUCCESS!!");
    }
    PRETTY_PRINT("ALL GOOD");
}

uint32_t Device::_get_default_index(void) {
    return 0;
}

VkPhysicalDevice Device::_get_default_device(void) {
    return 0;
}

AppResult Device::addExtension(const char *ext) {
    _m_extensions.push_back(ext); 
}

AppResult Device::init(void) {
    /* calculate family index */
    _m_queue_family_index = _get_default_index();

    /* queue Creation */
    VkDeviceQueueCreateInfo queue_create_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        /* 
         * TODO: determine index by a function such as _dev_get_graphics_queue_index() 
         */
        .queueFamilyIndex = _m_queue_family_index,
        .queueCount = 1,
    };

    VkDeviceCreateInfo devCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pQueueCreateInfos = &queue_create_info,
    };

    devCreateInfo.ppEnabledExtensionNames = _m_extensions.data();
    devCreateInfo.enabledExtensionCount = _m_extensions.size();

    if (VK_SUCCESS != vkCreateDevice(m_physical_device.getVkPhysicalDevice(), &devCreateInfo, nullptr, &_m_device)) {
        DBG_ERR("NOT SUCCESS!!");
    }

    vkGetDeviceQueue(_m_device, _m_queue_family_index, _m_queue_family_index, &_m_queue);
    
    PRETTY_PRINT("Logical device created succesfully");
}

void Device::wait() {
    VkResult res = vkDeviceWaitIdle(_m_device);
    assert(res == VK_SUCCESS);
}

/* TODO: add a check for valid device init */
VkQueue Device::getDeviceQueue() {
    return _m_queue;
}

uint32_t Device::getQueueFamilyIndex() {
    return _m_queue_family_index;
}

VkDevice Device::getVkDevice() {
    return _m_device;
}

VkPhysicalDevice Device::get_vk_physical_dev() {
    return m_physical_device.getVkPhysicalDevice();
}


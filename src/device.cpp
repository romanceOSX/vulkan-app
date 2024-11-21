#include <iostream>

#include "vulkan/vulkan_core.h"

#include "host.h"
#include "device.h"

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


#include <iostream>
#include <vulkan/vulkan_core.h>

#include "app.h"
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

    /* Physical Groups */
    getPhysicalDeviceGroups();

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

void Device::getPhysicalDeviceGroups() {
    uint32_t devGroupsCount;
    std::vector<VkPhysicalDeviceGroupProperties> devGroups;

    vkEnumeratePhysicalDeviceGroups(m_instance, &devGroupsCount, devGroups.data());
    devGroups.resize(devGroupsCount);
    vkEnumeratePhysicalDeviceGroups(m_instance, &devGroupsCount, devGroups.data());
    
    PRETTY_PRINT("Device Groups");
    for (auto& devGroup: devGroups) {
        std::cout << "Device Count: " << devGroup.physicalDeviceCount << std::endl;
    }
}

uint32_t Device::_get_default_index(void) {
    return 0;
}

VkPhysicalDevice Device::_get_default_device(void) {
    return 0;
}

PhysicalDevice::PhysicalDevice(VkPhysicalDevice dev): _m_physical_dev(dev) {
    /* Get device properties */
    _queryDeviceProperties();
    _queryQueueProperties();
    _queryDeviceExtensions();
}

void PhysicalDevice::_queryDeviceProperties() {
    vkGetPhysicalDeviceProperties(_m_physical_dev, &_m_physical_dev_props);
}

void PhysicalDevice::_queryQueueProperties() {
    vkGetPhysicalDeviceQueueFamilyProperties(_m_physical_dev, &_m_queueCount, nullptr);
    _m_available_queues.resize(_m_queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(_m_physical_dev, &_m_queueCount, _m_available_queues.data());
}

void PhysicalDevice::_queryDeviceExtensions() {
    uint32_t prop_count;
    vkEnumerateDeviceExtensionProperties(_m_physical_dev, nullptr, &prop_count, nullptr);
    _m_available_extensions.resize(prop_count);
    vkEnumerateDeviceExtensionProperties(_m_physical_dev, nullptr, &prop_count, _m_available_extensions.data());
}

std::vector<VkQueueFamilyProperties>& PhysicalDevice::getDeviceQueueProperties() {
    return _m_available_queues;
}

std::vector<VkExtensionProperties>& PhysicalDevice::getDeviceExtensions() {
    return _m_available_extensions;
}

uint32_t PhysicalDevice::getQueueCount(void) {
    return _m_queueCount;
}

VkPhysicalDeviceProperties& PhysicalDevice::getDeviceProperties(void) {
    return _m_physical_dev_props;  
}


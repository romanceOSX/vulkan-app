#include <iostream>

#include "vulkan/vulkan_core.h"

#include "physical_device.h"

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

VkPhysicalDevice PhysicalDevice::getVkPhysicalDevice(void) {
    return _m_physical_dev;
}


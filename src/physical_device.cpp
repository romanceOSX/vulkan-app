#include <iostream>

#include "vulkan/vulkan_core.h"

#include "physical_device.h"

PhysicalDevice::PhysicalDevice(VkPhysicalDevice dev): m_vk_physical_device(dev) {
    /* Get device properties */
    _queryDeviceProperties();
    _queryQueueProperties();
    _queryDeviceExtensions();
}

void PhysicalDevice::_queryDeviceProperties() {
    vkGetPhysicalDeviceProperties(m_vk_physical_device, &m_vk_physical_device_props);
}

void PhysicalDevice::_queryQueueProperties() {
    vkGetPhysicalDeviceQueueFamilyProperties(m_vk_physical_device, &m_queue_family_count, nullptr);
    m_vk_queue_families.resize(m_queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(m_vk_physical_device, &m_queue_family_count, m_vk_queue_families.data());
}

void PhysicalDevice::_queryDeviceExtensions() {
    uint32_t prop_count;
    vkEnumerateDeviceExtensionProperties(m_vk_physical_device, nullptr, &prop_count, nullptr);
    _m_available_extensions.resize(prop_count);
    vkEnumerateDeviceExtensionProperties(m_vk_physical_device, nullptr, &prop_count, _m_available_extensions.data());
}

std::vector<VkQueueFamilyProperties>& PhysicalDevice::getDeviceQueueProperties() {
    return m_vk_queue_families;
}

std::vector<VkExtensionProperties>& PhysicalDevice::getDeviceExtensions() {
    return _m_available_extensions;
}

uint32_t PhysicalDevice::getQueueCount(void) {
    return m_queue_family_count;
}

VkPhysicalDeviceProperties& PhysicalDevice::getDeviceProperties(void) {
    return m_vk_physical_device_props;  
}

VkPhysicalDevice PhysicalDevice::getVkPhysicalDevice(void) {
    return m_vk_physical_device;
}


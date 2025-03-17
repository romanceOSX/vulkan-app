#include <iostream>
#include <iterator>
#include <string>
#include <set>

#include "vulkan/vulkan_core.h"

#include "app_settings.hpp"
#include "physical_device.hpp"
#include "window.hpp"

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
    m_available_extensions.resize(prop_count);
    vkEnumerateDeviceExtensionProperties(m_vk_physical_device, nullptr, &prop_count, m_available_extensions.data());
}

std::vector<VkQueueFamilyProperties>& PhysicalDevice::getDeviceQueueProperties() {
    return m_vk_queue_families;
}

std::vector<VkExtensionProperties>& PhysicalDevice::getDeviceExtensions() {
    return m_available_extensions;
}

uint32_t PhysicalDevice::getQueueCount(void) {
    return m_queue_family_count;
}

VkPhysicalDeviceProperties& PhysicalDevice::getDeviceProperties(void) {
    return m_vk_physical_device_props;  
}

VkPhysicalDevice PhysicalDevice::get_vk_physical_device(void) {
    return m_vk_physical_device;
}

void PhysicalDevice::print_info() {
    std::cout << "Available queue families: " << std::size(m_vk_queue_families) << std::endl;
    /* print device name and info */
    for (auto& queue: m_vk_queue_families) {
        std::cout << "--Number of queues: " << queue.queueCount << std::endl;
        std::cout << "  Queue flags: " << queue.queueFlags << std::endl;
    }
}

/*
 * TODO: This function will look only for the queue supporting both presentation and
 * graphics capabilities, add support for choosing a different queue or the same one
 */
/* TODO: check copy constructor of std::optional */
std::optional<uint32_t> PhysicalDevice::get_suitable_queue_index(Window& window) {
    std::vector<QueueFamily> queue_families;

    std::vector<const char*> required_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME, 
    };

    /* Check if device has VK_KHR_DISPLAY_SWAPCHAIN_EXTENSION_NAME ext */
    bool is_swapchain_supported = false;

    std::set<std::string> required_set{std::begin(required_extensions), std::end(required_extensions)};
    for (const auto& ext: m_available_extensions) {
        required_set.erase(ext.extensionName);
    }

    if (!(is_swapchain_supported = required_set.empty())) {
        return {};
    }

    for (size_t i = 0; i < std::size(m_vk_queue_families); i++) {
        queue_families.emplace_back(m_vk_queue_families.at(i), i, *this, window);
    }

    /* We need a device that has both the graphics, and presnetation bit */
    for (auto queue_family: queue_families) {
        if ((true == queue_family.is_presentation()) && (true == queue_family.is_graphics())) {
            return queue_family.get_index();
        }
    }

    return {};
}

/*
 * Wrapper class for QueueFamily
 */
QueueFamily::QueueFamily(VkQueueFamilyProperties& queue_family, uint32_t family_index, PhysicalDevice& phy_dev, Window& window):
    m_vk_queue_family(queue_family),
    m_queue_family_index(family_index),
    m_physical_device(phy_dev),
    m_window(window) 
{
    /* check if queue has graphic bit */
    if (m_vk_queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        m_is_graphics = true;
    }

    /* check for presentation */
    VkBool32 is_suitable;
    vkGetPhysicalDeviceSurfaceSupportKHR(
            m_physical_device.get_vk_physical_device(),
            m_queue_family_index,
            m_window.get_vk_surface(),
            &is_suitable
    );
    m_is_presentation = static_cast<bool>(is_suitable);
}

bool QueueFamily::is_graphics() {
    return m_is_graphics;
}

bool QueueFamily::is_presentation() {
    return m_is_presentation;
}

uint32_t QueueFamily::get_index() {
    return m_queue_family_index;
}


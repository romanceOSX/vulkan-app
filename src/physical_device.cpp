#include <iostream>
#include <iterator>
#include <string>
#include <set>

#include "vulkan/vulkan_core.h"

#include "app_settings.hpp"
#include "physical_device.hpp"
#include "window.hpp"

PhysicalDevice::PhysicalDevice(VkPhysicalDevice dev): m_vk_physical_device(dev) {
    APP_PRETTY_PRINT_CUSTOM("creating physical device wrapper...", "☀️");
    /* Get device properties */
    _query_physical_device_properties();
    _query_physical_device_extensions();
    _query_queue_families();
}

void PhysicalDevice::_query_physical_device_properties() {
    vkGetPhysicalDeviceProperties(m_vk_physical_device, &m_vk_physical_device_properties);
}

void PhysicalDevice::_query_queue_families() {
    vkGetPhysicalDeviceQueueFamilyProperties(m_vk_physical_device, &m_queue_families_count, nullptr);
    m_vk_queue_families.resize(m_queue_families_count);
    vkGetPhysicalDeviceQueueFamilyProperties(m_vk_physical_device, &m_queue_families_count, m_vk_queue_families.data());
    
    /* create queuefamily wrappers */
}

void PhysicalDevice::_query_physical_device_extensions() {
    uint32_t prop_count;
    vkEnumerateDeviceExtensionProperties(m_vk_physical_device, nullptr, &prop_count, nullptr);
    m_vk_physical_device_extensions.resize(prop_count);
    vkEnumerateDeviceExtensionProperties(m_vk_physical_device, nullptr, &prop_count, m_vk_physical_device_extensions.data());
}

std::vector<VkQueueFamilyProperties>& PhysicalDevice::get_vk_device_queue_families_properties() {
    return m_vk_queue_families;
}

std::vector<VkExtensionProperties>& PhysicalDevice::get_vk_physical_device_extensions() {
    return m_vk_physical_device_extensions;
}

uint32_t PhysicalDevice::get_queue_families_count(void) {
    return m_queue_families_count;
}

VkPhysicalDeviceProperties& PhysicalDevice::get_vk_physical_device_properties(void) {
    return m_vk_physical_device_properties;  
}

VkPhysicalDevice PhysicalDevice::get_vk_physical_device(void) {
    return m_vk_physical_device;
}

void PhysicalDevice::print_info() {
    APP_PRINT_INFO("Physical Device info:");
    std::cout << "Available queue families: " << m_queue_families_count << std::endl;
    /* TODO: print available extensions, but too cluttered */
    /* print device name and info */
    std::cout << "Queue families:" << std::endl;
    for (auto& queue_family: m_queue_families) {
        queue_family.print_info();
    }
}

/*
 * TODO: This function will look only for the queue supporting both presentation and
 * graphics capabilities, add support for choosing a different queue or the same one
 */
/* TODO: check copy constructor of std::optional */
std::optional<uint32_t> PhysicalDevice::get_suitable_queue_family_index(Window& window) {
    std::vector<const char*> required_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME, 
    };

    /* Check if device has VK_KHR_DISPLAY_SWAPCHAIN_EXTENSION_NAME ext */
    bool is_swapchain_supported = false;

    std::set<std::string> required_set{std::begin(required_extensions), std::end(required_extensions)};
    for (const auto& ext: m_vk_physical_device_extensions) {
        required_set.erase(ext.extensionName);
    }

    if (!(is_swapchain_supported = required_set.empty())) {
        return {};
    }

    for (size_t i = 0; i < std::size(m_vk_queue_families); i++) {
        m_queue_families.emplace_back(m_vk_queue_families.at(i), i, *this, window);
    }

    /* We need a device that has both the graphics, and presnetation bit */
    for (auto queue_family: m_queue_families) {
        if ((true == queue_family.is_presentation()) && (true == queue_family.is_graphics())) {
            return queue_family.get_index();
        }
    }

    return {};
}

/*
 * Wrapper class for QueueFamily
 */
QueueFamily::QueueFamily(VkPhysicalDevice physical_device, VkQueueFamilyProperties& queue_family, uint32_t index):
    m_vk_queue_family_properties{queue_family},
    m_vk_physical_device{physical_device},
    m_queue_family_index{index}
{ }

void QueueFamily::query_surface_support(Window& window) {
    /* check if queue has graphic bit */
    if (m_vk_queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
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

void QueueFamily::print_info() {
    std::cout << "etesech" << std::endl;
}


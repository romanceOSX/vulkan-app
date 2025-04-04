#include <iostream>
#include <iterator>
#include <stdexcept>
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

    /* create queue family wrappers */
    uint32_t index = 0;
    for (auto& vk_queue_family: m_vk_queue_families) {
        m_queue_families.emplace_back(m_vk_physical_device, vk_queue_family, index++);   
    }
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

    /* 
     *  check if device even supports the swapchain extension
     *  "VK_KHR_DISPLAY_SWAPCHAIN_EXTENSION_NAME"
     */ 
    bool is_swapchain_supported = false;
    std::set<std::string> required_set{std::begin(required_extensions), std::end(required_extensions)};
    for (const auto& ext: m_vk_physical_device_extensions) {
        required_set.erase(ext.extensionName);
    }

    if (!(is_swapchain_supported = required_set.empty())) {
        return {};
    }

    /* 
     * query swapchain details
     * (needed at the swapchain creation stage)
     * TODO: add note here on intersection between window, surface, and device
     */

    /* query surface capabilities */
    VkSurfaceCapabilitiesKHR vk_surface_capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            m_vk_physical_device,
            window.get_vk_surface(),
            &m_swapchain_support_details.capabilities
    );

    /* query surface formats */
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_vk_physical_device, window.get_vk_surface(), &format_count, nullptr);
    if (0 != format_count) {
        m_swapchain_support_details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
                m_vk_physical_device,
                window.get_vk_surface(),
                &format_count,
                m_swapchain_support_details.formats.data()
        );
    }

    /* query present modes */
    uint32_t mode_count;
   vkGetPhysicalDeviceSurfacePresentModesKHR(m_vk_physical_device, window.get_vk_surface(), &mode_count, nullptr); 
    if (0 != mode_count) {
        m_swapchain_support_details.preset_modes.resize(mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
                m_vk_physical_device,
                window.get_vk_surface(),
                &mode_count,
                m_swapchain_support_details.preset_modes.data()
        );
    }

    /*
     * for this application, support is valid if there is at least 
     * one supported image format and one supported presentation mode
     */
    bool is_swap_chain_adequate = false;
    is_swap_chain_adequate = !m_swapchain_support_details.formats.empty() 
        && !m_swapchain_support_details.preset_modes.empty();

    if (!is_swap_chain_adequate) {
        throw std::invalid_argument("Physical device not supported by surface");
    }

    /*
     * check if the physical device has the required queue families
     * We need the device to have a queue family with both:
     *  - graphics bit
     *  - presentation bit
     */
    for (auto queue_family: m_queue_families) {
        queue_family.query_surface_support(window);
        if ((true == queue_family.is_presentation()) && (true == queue_family.is_graphics())) {
            return queue_family.get_index();
        }
    }

    return {};
}

SwapchainSupportDetails& PhysicalDevice::get_swapchain_support_details() {
    return m_swapchain_support_details;
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
            m_vk_physical_device,
            m_queue_family_index,
            window.get_vk_surface(),
            &is_suitable
    );

    m_is_presentation = static_cast<bool>(is_suitable);

    m_is_presentation_queried = true;
}

bool QueueFamily::is_graphics() {
    if (!m_is_presentation_queried) { throw std::runtime_error("Window surface support was not queried 😵"); }
    return m_is_graphics;
}

bool QueueFamily::is_presentation() {
    if (!m_is_presentation_queried) { throw std::runtime_error("Window surface support was not queried 😵"); }
    return m_is_presentation;
}

uint32_t QueueFamily::get_index() {
    return m_queue_family_index;
}

void QueueFamily::print_info() {
    APP_PRINT_INFO("Queue Family info:");
    std::cout << "Queue index: " << m_queue_family_index << std::endl;;
    std::cout << "Queue family flags: " << m_vk_queue_family_properties.queueFlags << std::endl;
    std::cout << "Number of queues: " << m_vk_queue_family_properties.queueCount << std::endl;

    /*
     * 7 equals:
     *  VK_QUEUE_GRAPHICS_BIT = 0x00000001,
     *  VK_QUEUE_COMPUTE_BIT = 0x00000002,
     *  VK_QUEUE_TRANSFER_BIT = 0x00000004,
     */
}


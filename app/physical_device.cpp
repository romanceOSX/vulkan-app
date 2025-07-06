#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <set>

#include "vulkan/vulkan_core.h"

#include "app_settings.hpp"
#include "physical_device.hpp"
#include "window.hpp"

/*
 * A physical device contains queue families,
 * Queue families contain an specified number of available queues
 * We create the required queues from an specific queue family while creating logical devices
 */

/*
 * VkPhysicalDevice wrapper class
 */
PhysicalDevice::PhysicalDevice(VkPhysicalDevice dev): m_vk_physical_device(dev) {
    /* Get device properties */
    _query_physical_device_properties();
    _query_physical_device_extensions();
    _query_physical_device_memory_properties();
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
 * This function performs the following checks:
 *  - overall device compatiblity with the given window surface
 *  - looks for a queue family that is compatible with the given window surface
 *  - looks for a queue that contains graphics capabilities
 */
std::optional<QueueFamily> PhysicalDevice::is_window_surface_compatible(Window& window) {
    std::vector<const char*> required_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME, 
    };

    /* 
     *  We check if the physical device is capable of presenting stuff to the screen
     *      Check if device even supports the swapchain extension
     *      "VK_KHR_DISPLAY_SWAPCHAIN_EXTENSION_NAME"
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
     * query surface details and capabilities
     * (needed at the swapchain creation stage)
     *  - basic surface capabilities:
     *      - min/max number of images in swapchain
     *      - min/max width/height of images
     *  - surface formats
     *      - pixel formats
     *      - color space
     *  - presentation modes
     */

    /* query surface capabilities */
    VkSurfaceCapabilitiesKHR vk_surface_capabilities;
    if (VK_SUCCESS != vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            m_vk_physical_device,
            window.get_vk_surface(),
            &m_swapchain_support_details.capabilities
    ))
    {
        throw std::runtime_error("Couldn't query the surface capabilities 😵");
    };

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
        throw std::runtime_error("Physical device not supported by surface");
    }

    /*
     * check if the physical device has the required queue families
     * We need the device to have a queue family with both:
     *  - graphics bit
     *  - presentation bit
     */
    for (auto& queue: m_queue_families) {
        if (
            /* supports graphics */
            queue.is_flag_supported(VK_QUEUE_GRAPHICS_BIT) == true &&
            /* window is presentable */
            queue.is_window_supported(window)
        ) 
        {
            return _pop_queue_family(queue);
        }
    }

    return {};
}

QueueFamily PhysicalDevice::_pop_queue_family(QueueFamily& queue) {
    for (auto it = std::begin(m_queue_families); it != std::end(m_queue_families); ++it) {
        /* it is the same queue family */
        if (queue.get_index() == queue.get_index()) {
            auto ret_queue = std::move(*it);
            m_queue_families.erase(it);
            return ret_queue;
        }
    }
}

SwapchainSupportDetails& PhysicalDevice::get_swapchain_support_details() {
    return m_swapchain_support_details;
}

void PhysicalDevice::_query_physical_device_memory_properties() {
    vkGetPhysicalDeviceMemoryProperties(m_vk_physical_device, &m_vk_physical_device_memory_properties);
}


VkPhysicalDeviceMemoryProperties PhysicalDevice::get_vk_physical_device_memory_properties(void) {
    return m_vk_physical_device_memory_properties;
}

/*
 * Finds the memory type that satisties the requested properties
 *
 * @param type_mask             The type of memory indexes mask to look the properties in
 * @param required_properties   Properties to find
 * @return                      optional index of the memory type
 */
std::optional<uint32_t> PhysicalDevice::find_memory_properties(uint32_t type_mask, VkMemoryPropertyFlags required_properties) {
    /* you can think of it as, "I accept these certain memory types, check if they also support the requested properties" */
    /* different memory types exist within the memory heaps */
    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(m_vk_physical_device, &mem_props);

    uint32_t mem_type_count = get_vk_physical_device_memory_properties().memoryTypeCount;
    for (uint32_t mem_index = 0; mem_index < mem_type_count; ++mem_index) {
        uint32_t mem_type_bits = (1 << mem_index);
        bool is_required_mem_type = type_mask & mem_type_bits;

        VkMemoryPropertyFlags properties = 
            get_vk_physical_device_memory_properties().memoryTypes[mem_index].propertyFlags;
        bool has_required_properties =
            (properties & required_properties) == required_properties;

        if (is_required_mem_type && has_required_properties) {
            return static_cast<uint32_t>(mem_index);
        }
    }

    return {};
}

/*
 * Finds the required queue family
 */
std::optional<QueueFamily> PhysicalDevice::find_queue_family(VkQueueFlagBits flag) {
    for (auto it = std::begin(m_queue_families); it != std::end(m_queue_families); ++it) {
        if ((*it).is_flag_supported(flag) == true) {
            return _pop_queue_family(*it);
        }
    }
    return {};
}

/*
 * Wrapper class for VkQueueFamilyProperties
 */
QueueFamily::QueueFamily(VkPhysicalDevice physical_device, VkQueueFamilyProperties queue_family, uint32_t index):
    m_vk_queue_family_properties{queue_family},
    m_vk_physical_device{physical_device},
    m_queue_family_index{index}
{ }

/* 
 * Checks if the queue family supports the given window surface
 */
bool QueueFamily::is_window_supported(Window& window) {
    VkBool32 is_suitable;

    vkGetPhysicalDeviceSurfaceSupportKHR(
            m_vk_physical_device,
            m_queue_family_index,
            window.get_vk_surface(),
            &is_suitable
    );

    return static_cast<bool>(is_suitable);
}

/*
 * gets the queue family index 
 */
uint32_t QueueFamily::get_index() const {
    return m_queue_family_index;
}

/*
 * Pretty prints the queue family information
 */
void QueueFamily::print_info() {
    APP_PRINT_INFO("Queue Family info:");
    std::cout << "Queue index: " << m_queue_family_index << std::endl;
    std::cout << "Queue family flags: " << m_vk_queue_family_properties.queueFlags << std::endl;
    std::cout << "Number of queues: " << m_vk_queue_family_properties.queueCount << std::endl;

    /*
     * 7 equals:
     *  VK_QUEUE_GRAPHICS_BIT = 0x00000001,
     *  VK_QUEUE_COMPUTE_BIT = 0x00000002,
     *  VK_QUEUE_TRANSFER_BIT = 0x00000004,
     */
}

/*
 * Returns the number of queues in the queue family
 */
uint32_t QueueFamily::count() const {
    return m_vk_queue_family_properties.queueCount;
}

/*
 * Checks if the specified queue flags are supported
 */
bool QueueFamily::is_flag_supported(VkQueueFlagBits flag) {
    return static_cast<bool>(
        m_vk_queue_family_properties.queueFlags & flag
    );
}


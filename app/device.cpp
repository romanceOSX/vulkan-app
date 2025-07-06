#include <iostream>
#include <cassert>
#include <stdexcept>

#include "vulkan/vulkan_core.h"

#include "app_settings.hpp"
#include "host.hpp"
#include "device.hpp"
#include "physical_device.hpp"
#include "window.hpp"

/*
 * A device represents a logical connection to the physical device
 * A physical device exposes a number of queue families,
 * each queue family contains a certain number of queues
 *
 * Both queues and devices are created at the same time
 *
 * Device uses
 *  --> https://docs.vulkan.org/spec/latest/chapters/devsandqueues.html#devsandqueues-use
 *  A device is used in the following vulkan operations:
 *      - Creation of Queues
 *      - Creation of synchronization constructs
 *      - Allocating, Free'ing and managing memory
 *      - Creation and destruction of command buffers
 *      - Creation destruction and management of graphics state (Pipeline State Objects)
 *      - etc...
 *
 * The queue family index is the piece of information used to relate different
 * vulkan constructs together:
 *  - queue families
 *  - vkGetDeviceQueue
 *  - command pools
 *  - command buffers
 *  - resources (images/buffers)
 *
 */

/*
 * A physical device contains queue families,
 * Queue families contain an specified number of available queues
 * We create the required queues from an specific queue family while creating logical devices
 */

Device::Device(PhysicalDevice& dev, Window& window): m_physical_device{dev}, m_window{window} { }

uint32_t Device::_get_suitable_queue_index(void) {
    /* TODO: add a cleaner way to query suitable indexes */
    if (auto ret = m_physical_device.check_window_surface_compatibility(m_window)) {
        return ret.value().get_index();
    }
    throw std::runtime_error("Physical Device not compatible with window 😵");
}

void Device::add_extension(const char *ext) {
    m_extensions.push_back(ext); 
}

void Device::init(uint32_t count) {
    /* calculate family index */
    /* TODO: we can pass the queuefamily wrapper instead of the index */
    m_queue_family_index = _get_suitable_queue_index();
    m_queue_count = count;

    if (count > m_physical_device.get_vk_device_queue_families_properties().at(m_queue_family_index).queueCount) {
        throw std::runtime_error("Requesting more queues than available 😵");
    }

    float queue_priority = 1.0f;

    /* queue Creation */
    VkDeviceQueueCreateInfo queue_create_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = m_queue_family_index,
            .queueCount = m_queue_count,
            .pQueuePriorities = &queue_priority,
    };

    /* TODO: VkDeviceQueueCreateInfo could be an array of desired queue creations */
    VkDeviceCreateInfo device_create_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_create_info,
        .pEnabledFeatures = VK_FALSE,
    };

    device_create_info.ppEnabledExtensionNames = m_extensions.data();
    device_create_info.enabledExtensionCount = m_extensions.size();

    if (VK_SUCCESS != vkCreateDevice(m_physical_device.get_vk_physical_device(), &device_create_info, nullptr, &m_vk_device)) {
        APP_DBG_ERR("Failed to create device");
    }

    vkGetDeviceQueue(m_vk_device, m_queue_family_index, 0, &m_vk_queue);
    m_is_init = true;
    APP_PRETTY_PRINT_CREATE("created logical device and requested queue");
}

void Device::wait() {
    VkResult res = vkDeviceWaitIdle(m_vk_device);
    assert(res == VK_SUCCESS);
}

/* TODO: add a check for valid device init */
VkQueue Device::get_vk_queue() {
    return m_vk_queue;
}

uint32_t Device::get_queue_family_index() {
    return m_queue_family_index;
}

VkDevice Device::get_vk_device() {
    return m_vk_device;
}

VkPhysicalDevice Device::get_vk_physical_dev() {
    return m_physical_device.get_vk_physical_device();
}

void Device::print_info() {
    if (!m_is_init) {
        throw std::runtime_error("Tried to print info of un-initialized logical device 😵");
    }

    APP_PRINT_INFO("Logical device info:");
    std::cout << "Choosen queue family index: " << m_queue_family_index << std::endl;
    std::cout << "Enabled device extensions: " << std::endl;
    for (const auto& extension: m_extensions) {
        std::cout << "- " << extension << std::endl;
    }
    std::cout << "Number of queue families spawned: " << m_queue_count << std::endl;
}

PhysicalDevice& Device::get_physical_device() {
    return m_physical_device;
}

/* user-defined conversion function */
Device::operator VkDevice() {
    return m_vk_device;
}

Device::~Device() {
    vkDestroyDevice(m_vk_device, nullptr);
    APP_PRETTY_PRINT_DESTROY("destroyed logical device and queue...");
}


#include <iostream>
#include <cassert>
#include <limits>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <utility>

#include "GLFW/glfw3.h"
#include "vulkan/vulkan_core.h"

#include "app_settings.hpp"
#include "host.hpp"
#include "device.hpp"
#include "physical_device.hpp"
#include "window.hpp"

/*
 * A physical device contains queue families,
 * Queue families contain an specified number of available queues
 * We create the required queues from an specific queue family while creating logical devices
 */

Device::Device(PhysicalDevice& dev, Window& window): m_physical_device{dev}, m_window{window} {
    APP_PRETTY_PRINT_CUSTOM("creating logical Device...", "☀️");
}

uint32_t Device::_get_suitable_queue_index(void) {
    if (auto ret = m_physical_device.get_suitable_queue_index(m_window)) {
        return ret.value();
    }
    throw;
}

void Device::add_extension(const char *ext) {
    m_extensions.push_back(ext); 
}

void Device::init(uint32_t count) {
    /* calculate family index */
    m_queue_family_index = _get_suitable_queue_index();
    m_queue_family_count = count;

    float queue_priority = 1.0f;

    /* queue Creation */
    VkDeviceQueueCreateInfo queue_create_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = m_queue_family_index,
        .queueCount = m_queue_family_count,
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
        APP_DBG_ERR("NOT SUCCESS!!");
    }

    vkGetDeviceQueue(m_vk_device, m_queue_family_index, 0, &m_vk_queue);
    m_is_init = true;
    
    APP_PRETTY_PRINT("Logical device created succesfully");
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

Device::~Device() {
    APP_PRETTY_PRINT_CUSTOM("Destroying logical device and queue...", "🌙");
    vkDestroyDevice(m_vk_device, nullptr);
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
    std::cout << "Number of queue families spawned: " << m_queue_family_count << std::endl;
}


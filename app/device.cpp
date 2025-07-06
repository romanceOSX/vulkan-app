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

/* TODO: we should add an init check to the device in a idiomatic way by overloading the bool operator */

/*
 * TODO: should all logical device-related operations be bound to the device class?
 *       Doing so in the current way (devices are separated from say, command pools, resources, etc...)
 *       means that we can bind different command pools to different devices, is this desired?
 */

Device::Device(PhysicalDevice& dev): m_physical_device{dev} {
}

/*
 * Adds required extension to the device
 */
void Device::add_extension(const char *ext) {
    m_extensions.push_back(ext); 
}

void Device::init() {
    /* create device and queues */
    VkDeviceCreateInfo device_create_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = static_cast<uint32_t>(m_dev_queue_create_infos.size()),
        .pQueueCreateInfos = m_dev_queue_create_infos.data(),
        .pEnabledFeatures = VK_FALSE,
    };

    device_create_info.ppEnabledExtensionNames = m_extensions.data();
    device_create_info.enabledExtensionCount = static_cast<uint32_t>(m_extensions.size());

    if (VK_SUCCESS != vkCreateDevice(m_physical_device.get_vk_physical_device(), &device_create_info, nullptr, &m_vk_device)) {
        APP_DBG_ERR("Failed to create device");
    }

    m_is_init = true;
    APP_PRETTY_PRINT_CREATE("created logical device and requested queue");
}

void Device::wait() {
    VkResult res = vkDeviceWaitIdle(m_vk_device);
    assert(res == VK_SUCCESS);
}

/* TODO: add a check for valid device init */
VkQueue Device::get_vk_queue(const QueueFamily& queue_family) {
    VkQueue vk_queue;
    vkGetDeviceQueue(m_vk_device, queue_family.get_index(), 0, &vk_queue);
    return vk_queue;
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
    std::cout << "Enabled device extensions: " << std::endl;
    for (const auto& extension: m_extensions) {
        std::cout << "- " << extension << std::endl;
    }
    std::cout << "Number of queue families spawned: " << m_dev_queue_create_infos.size() << std::endl;
}

PhysicalDevice& Device::get_physical_device() {
    return m_physical_device;
}

/*
 * Specifies a queue to create upon Device initialization
 */
void Device::add_queue(const QueueFamily& queue_family, uint32_t count, float priority) {
    /* --> https://docs.vulkan.org/spec/latest/chapters/devsandqueues.html#devsandqueues-queue-creation */

    /* TODO: add is_init check to see if someone tries to add a queue after creation */
    /* WARN: should we use something more idiomatic rather than an assert? */
    assert(count <= queue_family.count());

    VkDeviceQueueCreateInfo queue_create_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,                                                             /* TODO: should we also pass this as a parameter? */
        .queueFamilyIndex = queue_family.get_index(),
        .queueCount = count,
        .pQueuePriorities = &priority,
    };

    m_dev_queue_create_infos.push_back(queue_create_info);
}

/* user-defined conversion function */
Device::operator VkDevice() {
    return m_vk_device;
}

Device::~Device() {
    vkDestroyDevice(m_vk_device, nullptr);
    APP_PRETTY_PRINT_DESTROY("destroyed logical device and queue...");
}


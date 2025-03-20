#include <iostream>
#include <cassert>

#include "vulkan/vulkan_core.h"

#include "app_settings.hpp"
#include "host.hpp"
#include "device.hpp"
#include "physical_device.hpp"
#include "window.hpp"

uint32_t Device::_get_suitable_queue_index(void) {
    if (auto ret = m_physical_device.get_suitable_queue_index(m_window)) {
        return ret.value();
    }
    throw;
}

void Device::addExtension(const char *ext) {
    m_extensions.push_back(ext); 
}

AppResult Device::init(uint32_t count) {
    /* calculate family index */
    m_queue_family_index = _get_suitable_queue_index();

    float queue_priority = 1.0f;

    /* queue Creation */
    VkDeviceQueueCreateInfo queue_create_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = m_queue_family_index,
        .queueCount = count,
        .pQueuePriorities = &queue_priority,
    };
    /* TODO: VkDeviceQueueCreateInfo could be an array of desired queue creations */
    VkDeviceCreateInfo devCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_create_info,
    };

    devCreateInfo.ppEnabledExtensionNames = m_extensions.data();
    devCreateInfo.enabledExtensionCount = m_extensions.size();

    if (VK_SUCCESS != vkCreateDevice(m_physical_device.get_vk_physical_device(), &devCreateInfo, nullptr, &m_vk_device)) {
        APP_DBG_ERR("NOT SUCCESS!!");
    }

    vkGetDeviceQueue(m_vk_device, m_queue_family_index, 0, &m_vk_queue);
    
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

uint32_t Device::getQueueFamilyIndex() {
    return m_queue_family_index;
}

VkDevice Device::getVkDevice() {
    return m_vk_device;
}

VkPhysicalDevice Device::get_vk_physical_dev() {
    return m_physical_device.get_vk_physical_device();
}

Device::~Device() {
    vkDestroyDevice(m_vk_device, nullptr);
}

SwapChain::SwapChain(VkPhysicalDevice phy_dev, VkSurfaceKHR surface):
SwapChain::SwapChain(const VkPhysicalDevice phy_dev, Window& window):
            m_vk_phy_dev{phy_dev}, 
            m_window{window} 
{
    VkResult res;
    res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            m_vk_phy_dev,
            m_window.get_vk_surface(),
            &m_vk_surface_capabilities
    );
};

void SwapChain::print_info() {
    std::cout << "⛓️‍💥 Printing swapchain info" << std::endl
        << "Max Image Count: " << m_vk_surface_capabilities.maxImageCount << std::endl;
}


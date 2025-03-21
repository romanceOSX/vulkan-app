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
/*
 * Swapchain class implementations
 */
VkSurfaceFormatKHR SwapChain::_choose_swap_surface_format(std::vector<VkSurfaceFormatKHR>& available_formats) {
    /* (Format, Colorspace) => */
    for (const auto& available_format: available_formats) {
        if ((available_format.format == VK_FORMAT_B8G8R8A8_SRGB) &&
            (available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
            return available_format;           
        }
    }
    /* Fallback case */
    return available_formats.front();
}

VkPresentModeKHR SwapChain::_choose_swap_present_mode(std::vector<VkPresentModeKHR>& available_modes) {
    for (const auto& mode: available_modes) {
        if (VK_PRESENT_MODE_MAILBOX_KHR == mode) {
            return mode;
        }
    }
    /* guaranteed to be always available */
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::_choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()){
        return capabilities.currentExtent;
    } else {
        int width;
        int height;
        glfwGetFramebufferSize(m_window.get_glfw_window(), &width, &height);

        VkExtent2D actual_extent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
        actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actual_extent.width = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actual_extent;
    }
}

/* function that checks the swapchain support given a physical device */
/* TODO: this should be queried when choosing physical device */
void SwapChain::_query_swapchain_support() {
    /* query surface capabilities */
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            m_vk_phy_dev,
            m_window.get_vk_surface(),
            &m_vk_surface_capabilities
    );

/* We assume that phy_dev does support the swapchain, previously determined */
SwapChain::SwapChain(const VkPhysicalDevice phy_dev, Window& window):
            m_vk_phy_dev{phy_dev}, 
            m_window{window} 
{
    _query_swapchain_support();

}

void SwapChain::print_info() {
    std::cout << "⛓️‍💥 Printing swapchain info" << std::endl
        << "Max Image Count: " << m_vk_surface_capabilities.maxImageCount << std::endl;
}


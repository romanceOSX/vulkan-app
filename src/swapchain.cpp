
#include <algorithm>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "app_settings.hpp"
#include "swapchain.hpp"
#include "device.hpp"
#include "window.hpp"
#include "physical_device.hpp"

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

void SwapChain::_create_swapchain() {
    auto& swapchain_support_details = m_physical_device.get_swapchain_support_details();

    /* create swap chain */
    VkSurfaceFormatKHR surface_format = _choose_swap_surface_format(swapchain_support_details.formats);
    VkPresentModeKHR present_mode = _choose_swap_present_mode(swapchain_support_details.preset_modes);
    VkExtent2D extent = _choose_swap_extent(swapchain_support_details.capabilities);

    /* It is recommended to choose at least more than the minimum */
    uint32_t image_count = swapchain_support_details.capabilities.minImageCount + 1;
    
    if ((swapchain_support_details.capabilities.maxImageCount > 0) &&
            (image_count > swapchain_support_details.capabilities.maxImageCount))
    {
        image_count = swapchain_support_details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swap_create_info {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = m_window.get_vk_surface(),
        .minImageCount = image_count,
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    };

    /* get graphics and presentation queue indexes of phy_dev */
    /* we assume they are the same for the time being */
    swap_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swap_create_info.queueFamilyIndexCount = 0;
    swap_create_info.pQueueFamilyIndices = nullptr;
    swap_create_info.preTransform = swapchain_support_details.capabilities.currentTransform;
    swap_create_info.presentMode = present_mode;
    swap_create_info.clipped = VK_TRUE;
    swap_create_info.oldSwapchain = VK_NULL_HANDLE;

    /* create swap chain */
    if (vkCreateSwapchainKHR(m_device.get_vk_device(),
                &swap_create_info,
                nullptr,
                &m_vk_swapchain)) 
    {
        throw std::runtime_error("Failed to create swap chain 😵");
    }

    /* populate data members */
    m_vk_format = surface_format.format;
    m_vk_extent_2d = extent;
}

/* We assume that phy_dev does support the swapchain, previously determined */
/* TODO: add an assertion for device supporting swapchain */
SwapChain::SwapChain(Device& dev, Window& window):
    m_device{dev},
    m_physical_device{dev.get_physical_device()},
    m_window{window} 
{
    APP_PRETTY_PRINT_CUSTOM("creating Swapchain...", "☀️");
    /* initialize swap chain */
    _create_swapchain();

    /* query swap chain images */
    uint32_t image_count;
    vkGetSwapchainImagesKHR(m_device.get_vk_device(), m_vk_swapchain, &image_count, nullptr);
    /* We create an image view per each image available in the swapchain */
    m_vk_swapchain_images.resize(image_count);
    m_vk_swapchain_image_views.resize(image_count);
    vkGetSwapchainImagesKHR(m_device.get_vk_device(), m_vk_swapchain, &image_count, m_vk_swapchain_images.data());

    /* initialize image views */
    size_t i = 0;
    for (auto& image_view: m_vk_swapchain_image_views) {
        VkImageViewCreateInfo image_view_create_info {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = m_vk_swapchain_images.at(i),
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            /* WARN: Formate is not initialized here */
            .format = m_vk_format,
        };
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_device.get_vk_device(), &image_view_create_info, nullptr, &(image_view)) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image views! 😵");
        }
        i++;
    }
}

void SwapChain::print_info() {
    APP_PRINT_INFO("Swapchain Info:");
    auto& swapchain_support_details = m_physical_device.get_swapchain_support_details();
    std::cout << "Max image count: " << swapchain_support_details.capabilities.maxImageCount << std::endl;
}

VkFormat SwapChain::get_vk_format() {
    return m_vk_format;
}

VkExtent2D SwapChain::get_vk_extent_2d() {
    return m_vk_extent_2d;
}

std::vector<VkImage>& SwapChain::get_vk_images() {
    return m_vk_swapchain_images;
}

void SwapChain::_create_image_views() {
    m_vk_swapchain_image_views.resize(m_vk_swapchain_images.size());
}

VkSwapchainKHR SwapChain::get_vk_swapchain() {
    return m_vk_swapchain;
}

size_t SwapChain::size() {
    return m_vk_swapchain_image_views.size();
}

std::vector<VkImageView>& SwapChain::get_vk_image_views() {
    return m_vk_swapchain_image_views;
}

/* TODO: add a logging facility on this */
SwapChain::~SwapChain() {
    APP_PRETTY_PRINT_CUSTOM("Destroying image views...", "🌙");
    for (auto& image_view: m_vk_swapchain_image_views) {
        vkDestroyImageView(m_device.get_vk_device(), image_view, nullptr);
    }

    APP_PRETTY_PRINT_CUSTOM("Destroying swapchain...", "🌙");
    vkDestroySwapchainKHR(m_device.get_vk_device(), m_vk_swapchain, nullptr);
}


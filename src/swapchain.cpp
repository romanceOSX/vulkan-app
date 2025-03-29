
#include <algorithm>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "swapchain.hpp"
#include "device.hpp"
#include "window.hpp"

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

    std::vector<VkSurfaceFormatKHR> surface_formats;
    uint32_t format_count;
    std::vector<VkPresentModeKHR> present_modes;
    uint32_t mode_count;

    /* query supported formats */
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_vk_phy_dev, m_window.get_vk_surface(), &format_count, nullptr);
    if (0 != format_count) {
        surface_formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_vk_phy_dev, m_window.get_vk_surface(), &format_count, surface_formats.data());
    }

    /* query present modes */
   vkGetPhysicalDeviceSurfacePresentModesKHR(m_vk_phy_dev, m_window.get_vk_surface(), &mode_count, nullptr); 
    if (0 != mode_count) {
        present_modes.resize(mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_vk_phy_dev, m_window.get_vk_surface(), &mode_count, present_modes.data());
    }

    /* check for adequate swapchain */
    bool is_swap_chain_adequate = false;
    is_swap_chain_adequate = !surface_formats.empty() && !present_modes.empty();

    if (!is_swap_chain_adequate) {
        throw std::invalid_argument("Physical device not supported by surface");
    }

    /* create swap chain */
    VkSurfaceFormatKHR surface_format = _choose_swap_surface_format(surface_formats);
    VkPresentModeKHR present_mode = _choose_swap_present_mode(present_modes);
    VkExtent2D extent = _choose_swap_extent(m_vk_surface_capabilities);

    /* It is recommended to choose at least more than the minimum */
    uint32_t image_count = m_vk_surface_capabilities.minImageCount + 1;
    
    if ((m_vk_surface_capabilities.maxImageCount > 0) &&
            (image_count > m_vk_surface_capabilities.maxImageCount))
    {
        image_count = m_vk_surface_capabilities.maxImageCount;
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
    swap_create_info.preTransform = m_vk_surface_capabilities.currentTransform;
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
SwapChain::SwapChain(Device& dev, Window& window):
    m_device{dev},
    m_vk_phy_dev{dev.get_vk_physical_dev()}, 
    m_window{window} 
{
    /* initialize swap chain */
    _query_swapchain_support();

    /* query swap chain images */
    uint32_t image_count;
    vkGetSwapchainImagesKHR(m_device.get_vk_device(), m_vk_swapchain, &image_count, nullptr);
    m_vk_swapchain_images.resize(image_count);
    vkGetSwapchainImagesKHR(m_device.get_vk_device(), m_vk_swapchain, &image_count, m_vk_swapchain_images.data());

    size_t i = 0;
    for (auto& image: m_vk_swapchain_image_views) {
        VkImageViewCreateInfo image_view_create_info {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = m_vk_swapchain_images.at(i),
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
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

        if (vkCreateImageView(m_device.get_vk_device(), &image_view_create_info, nullptr, &(image)) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image views! 😵");
        }
        i++;
    }
}

void SwapChain::print_info() {
    std::cout << "⛓️‍💥 Printing swapchain info" << std::endl
        << "Max Image Count: " << m_vk_surface_capabilities.maxImageCount << std::endl;
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

/* TODO: add a logging facility on this */
SwapChain::~SwapChain() {
    for (auto& image_view: m_vk_swapchain_image_views) {
        vkDestroyImageView(m_device.get_vk_device(), image_view, nullptr);
    }
}


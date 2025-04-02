#pragma once

#include <iostream>
#include <vector>

#include "vulkan/vulkan.h"

/*
 * Forward declarations
 */
class Device;
class Window;

class SwapChain {
    public:
        SwapChain(Device& dev, Window& window);
        ~SwapChain();
        void print_info();
        VkFormat get_vk_format();
        VkExtent2D get_vk_extent_2d();
        std::vector<VkImage>& get_vk_images();
        std::vector<VkImageView>& get_vk_image_views();
        VkSwapchainKHR  get_vk_swapchain();
        size_t size();

    private:
        void _query_swapchain_support();
        VkSurfaceFormatKHR _choose_swap_surface_format(std::vector<VkSurfaceFormatKHR>& available_formats);
        VkPresentModeKHR _choose_swap_present_mode(std::vector<VkPresentModeKHR>& available_modes);
        VkExtent2D _choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);
        void _create_image_views();

        VkPhysicalDevice                m_vk_phy_dev;
        Window&                         m_window;
        VkSurfaceCapabilitiesKHR        m_vk_surface_capabilities;
        VkSwapchainKHR                  m_vk_swapchain;
        Device&                         m_device;
        std::vector<VkImage>            m_vk_swapchain_images;
        VkFormat                        m_vk_format;
        VkExtent2D                      m_vk_extent_2d;
        std::vector<VkImageView>        m_vk_swapchain_image_views;
};

class ImageView {
    public:
    private:
};


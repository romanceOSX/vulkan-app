#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "app_settings.hpp"

#include "device.hpp"
#include "framebuffers.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"

/*
 * Framebuffers are the most direct interface to image resources (VkImageViews)
 * They encapsulate such resources
 */

/* TODO: make this class more flexible, first determine use-cases */
Framebuffers::Framebuffers(Device& dev, SwapChain& swap, Pipeline& pipeline):
    m_device{dev},
    m_swapchain{swap},
    m_pipeline{pipeline}
{
    _create_framebuffers();
}

void Framebuffers::_create_framebuffers() {
    /* WARN: check the sizing operation */
    auto image_views = m_swapchain.get_vk_image_views();
    m_swapchain_frame_buffers.resize(image_views.size());

    /* we create a framebuffer per each of the swapchain's image views */
    for (size_t i = 0; i < image_views.size(); i++) {
        VkImageView attachments[] = {
            image_views.at(i),
        };

        VkFramebufferCreateInfo frame_buffer_info{};
        frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frame_buffer_info.pNext = nullptr;
        frame_buffer_info.flags = 0;
        frame_buffer_info.renderPass = m_pipeline.get_render_pass().get_vk_render_pass();
        frame_buffer_info.attachmentCount = 1;
        frame_buffer_info.pAttachments = attachments;
        frame_buffer_info.width = m_swapchain.get_vk_extent_2d().width;
        frame_buffer_info.height = m_swapchain.get_vk_extent_2d().height;
        frame_buffer_info.layers = 1;

        if (vkCreateFramebuffer(m_device.get_vk_device(), &frame_buffer_info, nullptr, &m_swapchain_frame_buffers.at(i))
                != VK_SUCCESS) 
        {
            throw std::runtime_error("Failed to create framebuffer 😵");
        }
        APP_PRETTY_PRINT_CREATE("created framebuffer");
    }
}

/* TODO: add destroy function */

std::vector<VkFramebuffer>& Framebuffers::get_swapchain_frame_buffers() {
    return m_swapchain_frame_buffers;
}

Framebuffers::~Framebuffers() {
    for (const auto& framebuffer: m_swapchain_frame_buffers) {
        vkDestroyFramebuffer(m_device.get_vk_device(),
                framebuffer,
                nullptr
        );
        APP_PRETTY_PRINT_DESTROY("destroyed framebuffer");
    }
}


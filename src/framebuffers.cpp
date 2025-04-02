#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "app_settings.hpp"

#include "device.hpp"
#include "framebuffers.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"


Framebuffers::Framebuffers(Device& dev, SwapChain& swap, Pipeline& pipeline):
    m_device{dev},
    m_swapchain{swap},
    m_pipeline{pipeline}
{
    _create_framebuffers();
}

void Framebuffers::_create_framebuffers() {
    APP_PRETTY_PRINT_CUSTOM("creating Pipeline...", "☀️");
    /* WARN: check the sizing operation */
    auto image_views = m_swapchain.get_vk_image_views();
    m_swapchain_frame_buffers.resize(image_views.size());

    for (size_t i = 0; i < image_views.size(); i++) {
        VkImageView attachments[] = {
            image_views.at(i),
        };

        VkFramebufferCreateInfo frame_buffer_info{};
        frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frame_buffer_info.pNext = nullptr;
        frame_buffer_info.renderPass = m_pipeline.get_render_pass().get_vk_render_pass();
        frame_buffer_info.attachmentCount = 1;
        frame_buffer_info.pAttachments = attachments;
        frame_buffer_info.width = m_swapchain.get_vk_extent_2d().width;
        frame_buffer_info.width = m_swapchain.get_vk_extent_2d().height;
        frame_buffer_info.layers = 1;

        if (vkCreateFramebuffer(m_device.get_vk_device(), &frame_buffer_info, nullptr, &m_swapchain_frame_buffers[i])
                != VK_SUCCESS) 
        {
            throw std::runtime_error("Failed to create framebuffer 😵");
        }
    }
}


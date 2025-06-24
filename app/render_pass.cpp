
#include "device.hpp"
#include "swapchain.hpp"
#include "render_pass.hpp"

/*
 * RenderPass class
 */
RenderPass::RenderPass(Device& device, SwapChain& swapchain):
    m_device{device},
    m_swapchain{swapchain}
{
    /*
     * Color attachment
     */
    /* if we had multiple attachments then this would be an array */
    VkAttachmentDescription color_attachment{};
    color_attachment.flags = 0;
    /* We should be matching the format of the underlying swapchain */
    color_attachment.format = m_swapchain.get_vk_format();
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    /*
     * Subpasses
     */
    VkAttachmentReference color_attachment_ref{};
    /* 
     * this ::attachment refers to the color_attachment, 
     * since we only have one then the index is 0
     */
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    /*
     * Render Pass
     */
    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.pNext = nullptr;
    render_pass_info.flags = 0;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    if (vkCreateRenderPass(m_device.get_vk_device(), &render_pass_info, nullptr, &m_vk_render_pass)
            != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass! 😵");
    }
}

VkRenderPass RenderPass::get_vk_render_pass() {
    return m_vk_render_pass;
}


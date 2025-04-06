#include <stdexcept>
#include <tuple>

#include <vulkan/vulkan_core.h>

#include "app_settings.hpp"
#include "command_pool.hpp"
#include "device.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"
#include "framebuffers.hpp"

/*
 * Command Pool class
 */
CommandPool::CommandPool(Device& dev): m_device{dev} {
    VkCommandPoolCreateInfo command_pool_create = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = m_device.get_queue_family_index(),
    };

    if (VK_SUCCESS != vkCreateCommandPool(
                dev.get_vk_device(), 
                &command_pool_create,
                nullptr,
                &m_command_pool)
       )
    {
        APP_DBG_ERR("Failed to create Command Pool");
    }
    APP_PRETTY_PRINT_CREATE("created command pool");
}

void CommandPool::reset() {
    vkResetCommandPool(
            m_device.get_vk_device(),
            m_command_pool,
            VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
    );
}

VkCommandPool CommandPool::get_vk_command_pool() {
    return m_command_pool;
}

Device& CommandPool::get_device() {
    return m_device;
}

CommandBuffer& CommandPool::create_command_buffer() {
    /* for the time being we should always create one */
    m_command_buffers.emplace_back(*this, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY); 
    return m_command_buffers.back();
}

CommandPool::~CommandPool() {
    vkDestroyCommandPool(
            m_device.get_vk_device(),
            m_command_pool,
            nullptr
    );
    APP_PRETTY_PRINT_DESTROY("destroyed command pool");
}

/*
 * TODO: Find a better way to pass the Device around 
 *  all classes in some way related to device, should have a reference to it
 *  as a member
 */

/*
 * Command Buffer class
 */
CommandBuffer::CommandBuffer(CommandPool& cmdPool, uint32_t count, Type type)
    :m_command_pool{cmdPool},
    m_count{count},
    m_device{cmdPool.get_device()}
{
    /* TODO: This vector could be initialized at the initializer list instead */
    m_command_buffers.resize(m_count);

    VkCommandBufferAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = m_command_pool.get_vk_command_pool(),
        .level = type,
        .commandBufferCount = m_count,
    };

    if ((vkAllocateCommandBuffers(
            m_device.get_vk_device(),
            &allocate_info,
            m_command_buffers.data()
            ))
            != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create command buffer 😵");
    }
    APP_PRETTY_PRINT_ALLOC("allocated command buffer");
}

/* Resets all, edit class upon solid use-case */
void CommandBuffer::reset() {
    for (auto& buf: m_command_buffers) {
        vkResetCommandBuffer(buf, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    }
}

std::vector<VkCommandBuffer>& CommandBuffer::get_command_buffers() {
    return m_command_buffers;
}

void CommandBuffer::free() {
    vkFreeCommandBuffers(m_device.get_vk_device(),
            m_command_pool.get_vk_command_pool(),
            m_command_buffers.size(),
            m_command_buffers.data()
    );
}

/* This starts a render pass */
/* TODO: find a better interface for this */
void CommandBuffer::begin_recording()
{
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,     /* does not apply here */
        .pInheritanceInfo = nullptr,
    };
    if (vkBeginCommandBuffer(m_command_buffers.at(0), &beginInfo)
            != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin recording buffer 😵");
    }
}

void CommandBuffer::begin_render_pass(uint32_t image_index, SwapChain& swapchain, RenderPass& render_pass, Framebuffers& framebuffers) {
    /* begin render pass */
    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.pNext = nullptr;
    render_pass_info.renderPass = render_pass.get_vk_render_pass();
    render_pass_info.framebuffer = framebuffers.get_swapchain_frame_buffers().at(image_index);
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = swapchain.get_vk_extent_2d();

    /* this is the clear (empty) color */
    VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(m_command_buffers.front(), &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::bind_pipeline(Pipeline& pipeline) {
    vkCmdBindPipeline(m_command_buffers.front(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get_vk_pipeline());  
}

void CommandBuffer::draw() {
    vkCmdDraw(m_command_buffers.front(), 3, 1, 0, 0);
}

void CommandBuffer::set_viewport_and_scissor(SwapChain& swapchain) {
    /* set scissor and viewport */
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchain.get_vk_extent_2d().width);
    viewport.height = static_cast<float>(swapchain.get_vk_extent_2d().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(m_command_buffers.front(), 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchain.get_vk_extent_2d();
    vkCmdSetScissor(m_command_buffers.front(), 0, 1, &scissor);
}

void CommandBuffer::end_render_pass() {
    vkCmdEndRenderPass(m_command_buffers.front());
}

void CommandBuffer::end_recording() {
    if (vkEndCommandBuffer(m_command_buffers.front()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer ❌📹");
    }
}
/*
 * Command Buffer submission
 */
void CommandBuffer::submit() {
    auto tup = std::make_tuple(1, "hello");
    auto [a, b] = tup;
}


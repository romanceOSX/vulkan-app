/* Vulkan sample application */

#include <iostream>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "app_settings.hpp"
#include "app.hpp"
#include "framebuffers.hpp"
#include "host.hpp"
#include "instance.hpp"
#include "physical_device.hpp"
#include "device.hpp"
#include "window.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"
#include "command_pool.hpp"
#include "synchronization.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include "vulkan/vulkan_metal.h"

int run_app() {
    VulkanApp* app = VulkanApp::getInstance();

    try {
        app->run_stub();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}

void _physical_device_test() {
    /* create instance with required extensions */
    auto instance = Instance();
    instance.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.add_extension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    instance.add_extension(VK_KHR_SURFACE_EXTENSION_NAME);
    instance.add_extension(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
    //instance.add_layer("VK_LAYER_LUNARG_api_dump");
    //instance.add_layer("VK_LAYER_KHRONOS_validation");
    instance.init();

    auto vk_phy_devs = instance.get_vk_devices();

    /* physical devices not VkPhysicalDevice */
    std::vector<PhysicalDevice> phy_devs;

    /* initialize physical device wrappers */
    for (auto& dev: vk_phy_devs) {
        phy_devs.emplace_back(dev);
    }

    /* create glfw window */
    Window window{instance};

    /* get first available device */
    auto physical_device = phy_devs.front();
    physical_device.print_info();

    /* get suitable queue family index */
    auto queue_family_index = physical_device.check_window_surface_compatibility(window);
    if (!queue_family_index) {
        throw std::runtime_error("Device does not have a suitable queue capabilities");
    }

    /* create logical device */
    Device device{physical_device, window};
    device.add_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    device.add_extension("VK_KHR_portability_subset"); /* Check vulkan spec for this */
    device.init(1);
    device.print_info();
    
    /* create swapchain */
    SwapChain swapchain{device, window};
    swapchain.print_info();

    /* create pipeline */
    Pipeline pipeline{device, swapchain};

    /* create Framebuffers */
    Framebuffers framebuffers{device, swapchain, pipeline};

    /* create Vertex buffers */
    // TODO: create vertex buffers
    VertexBuffer vertex_buffer{device};

    /* create Command buffers */
    CommandPool command_pool{device};
    std::vector<VkSemaphore> image_available_semaphores;
    std::vector<VkSemaphore> render_finished_semaphores;
    std::vector<VkFence> in_flight_fences;

    /* create objects needed per frame */
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        /* command buffers */
        command_pool.create_command_buffer();
        /* synchronization primitives */
        image_available_semaphores.push_back(Semaphore::create(device));
        render_finished_semaphores.push_back(Semaphore::create(device));
        in_flight_fences.push_back(Fence::create(device));
    }

    uint32_t current_frame = 0;
    auto command_buffers = command_pool.get_command_buffers();

    while (!glfwWindowShouldClose(window.get_glfw_window())) {
        glfwPollEvents();
        /* wait for previous frames, first fence is initialized as ready */
        vkWaitForFences(device.get_vk_device(), 1, &in_flight_fences.at(current_frame), VK_TRUE, UINT64_MAX);

        uint32_t image_index;
        vkAcquireNextImageKHR(
                device.get_vk_device(),
                swapchain.get_vk_swapchain(),
                UINT64_MAX,
                image_available_semaphores.at(current_frame),
                VK_NULL_HANDLE,
                &image_index
        );

        vkResetFences(device.get_vk_device(), 1, &in_flight_fences[current_frame]);

        /* record command buffer */
        command_buffers.at(current_frame).reset();
        command_buffers.at(current_frame).begin_recording();
        command_buffers.at(current_frame).cmd_begin_render_pass(image_index, swapchain, pipeline.get_render_pass(), framebuffers);
        command_buffers.at(current_frame).cmd_bind_pipeline(pipeline);
        command_buffers.at(current_frame).cmd_set_viewport_and_scissor(swapchain);
        command_buffers.at(current_frame).cmd_draw();
        command_buffers.at(current_frame).cmd_end_render_pass();
        command_buffers.at(current_frame).end_recording();

        /* prepare submit */
        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = nullptr;

        VkSemaphore wait_semaphores[] = {image_available_semaphores.at(current_frame)};
        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;

        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &(command_buffers.at(current_frame).get_command_buffers().front());

        VkSemaphore signal_semaphores[] = {render_finished_semaphores.at(current_frame)};
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        /* submit the command buffer to the queue */
        if (vkQueueSubmit(device.get_vk_queue(), 1, &submit_info, in_flight_fences.at(current_frame)) != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer 😵");
        }

        /* prepare to present frame */
        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.pNext = nullptr;

        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swapchains[] = {swapchain.get_vk_swapchain()};
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapchains;

        present_info.pImageIndices = &image_index;

        vkQueuePresentKHR(device.get_vk_queue(), &present_info);

        current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    /* wait for asynch operations to finish */
    device.wait();
}

void _test_glfw() {
    glfwInit();

    if (glfwVulkanSupported()) {
        std::cout << "VULKAN IS SUPPORTED" << std::endl;
    }

    uint32_t count;
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);

    if (nullptr != extensions) {
        std::cout << "AVAILABLE EXTENSIONS!" << std::endl;
        std::cout << "Count: " << count << std::endl;
        for (size_t i = 0; i < count; i++) {
            std::cout << *(extensions + i) << std::endl;
        }
    }
    
}

//void _shader_modules_test() {
//    Pipeline pipeline;
//}

int main(int argc, char *argv[]) {
    //run_app();
    _physical_device_test();
    //_test_glfw();
    //_shader_modules_test();

    return 0;
}


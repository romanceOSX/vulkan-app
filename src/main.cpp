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

VkSemaphore _create_semaphore(Device& dev) {
    VkSemaphore semaphore;
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_info.pNext = nullptr;
    semaphore_info.flags = 0;

    if (vkCreateSemaphore(dev.get_vk_device(), &semaphore_info, nullptr, &semaphore)
            != VK_SUCCESS) 
    {
        throw std::runtime_error("Semaphore creation failed! 😵");
    }
    return semaphore;
}

VkFence _create_fence(Device& dev) {
    VkFence fence;
    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.pNext = nullptr;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(dev.get_vk_device(), &fence_info, nullptr, &fence)
            != VK_SUCCESS) 
    {
        throw std::runtime_error("Fence creation failed! 😵");
    }
    return fence;
}

void _physical_device_test() {
    /* create instance with required extensions */
    auto instance = Instance();
    instance.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.add_extension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    instance.add_extension(VK_KHR_SURFACE_EXTENSION_NAME);
    instance.add_extension(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
    //instance.add_layer("VK_LAYER_LUNARG_api_dump");
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
    auto queue_family_index = physical_device.get_suitable_queue_index(window);
    if (!queue_family_index) {
        throw std::runtime_error("Device does not have a suitable queue capabilities");
    }

    /* create logical device */
    Device device{physical_device, window};
    device.addExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    device.init(1);
    
    /* create swapchain */
    SwapChain swapchain{device, window};
    swapchain.print_info();

    /* create pipeline */
    Pipeline pipeline{device, swapchain};

    /* create Framebuffers */
    Framebuffers framebuffers{device, swapchain, pipeline};


    /* create Command buffers */
    CommandPool command_pool{device};
    auto command_buffer = command_pool.create_command_buffer();
    command_buffer.begin_recording();

    /* draw frame */

    /* run window? */
    //window.wait_to_close_window();

    /* Create synchronization primitives */
    VkSemaphore image_available_semaphore = _create_semaphore(device);
    VkSemaphore render_finished_semaphore = _create_semaphore(device);
    VkFence in_flight_fence = _create_fence(device);


    while (!glfwWindowShouldClose(window.get_glfw_window())) {
        glfwPollEvents();
        /* wait for previous frames */
        vkWaitForFences(device.get_vk_device(), 1, &in_flight_fence, VK_TRUE, UINT64_MAX);
        vkResetFences(device.get_vk_device(), 1, &in_flight_fence);

        uint32_t image_index;
        vkAcquireNextImageKHR(
                device.get_vk_device(),
                swapchain.get_vk_swapchain(),
                UINT64_MAX,
                image_available_semaphore,
                VK_NULL_HANDLE,
                &image_index
        );

        std::cout << "Image index => " << image_index << std::endl; /* 0 */

        command_buffer.reset();
        command_buffer.begin_recording();
        command_buffer.begin_render_pass(image_index, swapchain, pipeline.get_render_pass(), framebuffers);
        command_buffer.end_render_pass();
        command_buffer.end_recording();

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore wait_semaphores[] = {image_available_semaphore};
        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submit_info.pNext = nullptr;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
    }
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


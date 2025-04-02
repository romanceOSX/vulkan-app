/* Vulkan sample application */

#include <iostream>
#include <stdexcept>
#include <vector>

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

    /* run window? */
    //window.wait_to_close_window();

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


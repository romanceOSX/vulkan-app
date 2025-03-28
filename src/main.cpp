/* Vulkan sample application */

#include <iostream>
#include <vector>

#include "app_settings.hpp"
#include "app.hpp"
#include "host.hpp"
#include "instance.hpp"
#include "physical_device.hpp"
#include "device.hpp"
#include "window.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"

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
    auto instance = Instance();
    instance.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.add_extension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    instance.add_extension(VK_KHR_SURFACE_EXTENSION_NAME);
    instance.add_extension(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
    instance.init();

    auto vk_phy_devs = instance.get_vk_devices();

    /* Create glfw window */
    auto window = Window(instance);

    /* Run window? */
    //window.wait_to_close_window();

    /* Physical devices not VkPhysicalDevice */
    std::vector<PhysicalDevice> phy_devs;

    for (auto& dev: vk_phy_devs) {
        phy_devs.emplace_back(dev);
    }
    
    /* find suitable device */
    for (auto& dev: phy_devs) {
        std::cout << "Printing physical device info:" << std::endl;
        dev.print_info();
        std::cout << "-- Is device suitable?: "  << std::endl;
        if (auto queue_family_index = dev.get_suitable_queue_index(window)) {
            std::cout << "true! at queue index: " 
                << std::boolalpha
                << queue_family_index.value() 
                << std::endl;

            APP_PRETTY_PRINT("Creating logical device");
            Device device{dev, window};
            device.addExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
            device.init(1);

            /* query swapchain properties */
            SwapChain swap{device, window};
            swap.print_info();

            /* create pipeline */
            Pipeline pipeline{device};
        }
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


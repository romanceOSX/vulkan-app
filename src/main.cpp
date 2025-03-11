/* Main Vulkan application */

#include <iostream>
#include <vector>
#include <iterator>

#include "app.hpp"
#include "instance.hpp"
#include "physical_device.hpp"
#include "device.hpp"
#include "window.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

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
    
    for (auto& dev: phy_devs) {
        std::cout << "Printing physical device info:" << std::endl;
        dev.print_info();
        std::cout << "-- Is device suitable?: "  << std::endl;
        if (auto queue_family_index = dev.get_suitable_queue_index(window)) {
            std::cout << "true! at queue index: " 
                << std::boolalpha
                << queue_family_index.value() 
                << std::endl;
        }
    }
}

int main(int argc, char *argv[]) {
    //run_app();
    _physical_device_test();

    return 0;
}


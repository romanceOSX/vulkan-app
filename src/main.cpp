/* Main Vulkan application */

#include <iostream>
#include <vector>
#include <iterator>

#include "app.h"
#include "instance.h"
#include "physical_device.h"
#include "device.h"
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

int main(int argc, char *argv[]) {
    //run_app();

    return 0;
}


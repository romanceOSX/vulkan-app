#include <cassert>
#include <iostream>
#include <memory> 
#include <stdexcept>
#include <expected>

#include "app.h"
#include "host.h"
#include "instance.h"
#include "device.h"
#include "physical_device.h"

VulkanApp* VulkanApp::getInstance() {
    if (nullptr == _m_app_instance) {
        _m_app_instance = new VulkanApp;
    }
    return _m_app_instance;
}

VulkanApp::~VulkanApp() {
    PRETTY_PRINT("Cleaning up VulkanApp ♻️");
    delete _m_app_instance;
}

void VulkanApp::run() {
    Host* host = Host::getInstance();
    Instance* vi = host->getVkInstance();

    /* Extensions needed for MacOS */
    vi->addExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    vi->addExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    /* Extra Extensions */
    vi->addExtension(VK_KHR_SURFACE_EXTENSION_NAME);
    /* Debug facility */
    //vi->addExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    //vi->addLayer("VK_LAYER_LUNARG_api_dump");

    assert((VK_SUCCESS == vi->init()));

    host->printHostInfo();
    //host->printPhysicalDevInfo();

    //VkPhysicalDevice *dev = Host::getDefaultDevice();

    Device dev{Host::getDefaultDevice()};
    //dev.addExtension("ext1");
    //dev.addExtension("ext2");
    //dev.addExtension("ext3");
    dev.init();
    dev.wait();
    CHECK();
    
    ///* Print host information */
    //app->getHostInfo();
    
    //auto dev = app->getDefaultPhysicalDevice();

    //Device dev(instance);
    delete host;

    /* Ideally run should be like this: */
    // _initVulkan();
    // _mainLoop();
    // _cleanup();
}

void VulkanApp::_initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#ifndef APP_RESIZABLE_WINDOW
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#endif /* !APP_RESIZABLE_WINDOW */

    _m_window = glfwCreateWindow(APP_WINDOW_WIDTH, APP_WINDOW_HEIGHT, APP_WINDOW_NAME, nullptr, nullptr);
}

void VulkanApp::_initVulkan() {

}

void VulkanApp::_mainLoop() {
    while (!glfwWindowShouldClose(_m_window)) {
        glfwPollEvents();
    }
}

void VulkanApp::_cleanup() {
    glfwDestroyWindow(_m_window);
    glfwTerminate();
}

void VulkanApp::run_stub() {
    _initWindow();
    _initVulkan();
    _mainLoop();
    _cleanup();
}


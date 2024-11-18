#include <iostream>
#include <memory> 
#include <stdexcept>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "device.h"
#include "instance.h"
#include "app.h"

int main (int argc, char *argv[]) {
    App* app = App::getInstance();
    Instance* vi = app->getVkInstance();

    /* Extensions needed for MacOS */
    vi->addExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    vi->addExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    /* Extra Extensions */
    vi->addExtension(VK_KHR_SURFACE_EXTENSION_NAME);
    //vi->addLayer("VK_LAYER_LUNARG_api_dump");

    if (VK_SUCCESS != vi->init()) {
        throw std::runtime_error("Instance creation Failed");
    }

    app->printHostInfo();
    
    ///* Print host information */
    //app->getHostInfo();
    
    //auto dev = app->getDefaultPhysicalDevice();

    //Device dev(instance);
    delete app;
    return 0;
}


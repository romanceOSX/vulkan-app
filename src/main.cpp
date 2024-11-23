#include <cassert>
#include <iostream>
#include <memory> 
#include <stdexcept>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "device.h"
#include "physical_device.h"
#include "instance.h"
#include "host.h"

int main (int argc, char *argv[]) {
    Host* host = Host::getInstance();
    Instance* vi = host->getVkInstance();

    /* Extensions needed for MacOS */
    vi->addExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    vi->addExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    /* Extra Extensions */
    vi->addExtension(VK_KHR_SURFACE_EXTENSION_NAME);
    //vi->addLayer("VK_LAYER_LUNARG_api_dump");

    assert((VK_SUCCESS == vi->init()));

    host->printHostInfo();
    //host->printPhysicalDevInfo();

    //VkPhysicalDevice *dev = Host::getDefaultDevice();

    Device dev{Host::getDefaultDevice()};
    dev.addExtension("ext1");
    dev.addExtension("ext2");
    dev.addExtension("ext3");
    dev.init();
    
    ///* Print host information */
    //app->getHostInfo();
    
    //auto dev = app->getDefaultPhysicalDevice();

    //Device dev(instance);
    delete host;
    return 0;
}


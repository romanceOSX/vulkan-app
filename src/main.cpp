#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "device.h"
#include "instance.h"
#include "app.h"

int main (int argc, char *argv[]) {
    App* app = App::getInstance();
    app->printHostInfo();
    //Instance* vi = app->getVkInstance();
    //vi->addExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    //vi->addExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    //vi->addExtension(VK_KHR_SURFACE_EXTENSION_NAME);
    //vi->addLayer("VK_LAYER_LUNARG_api_dump");
    //vi->init();

    ///* Print host information */
    //app->getHostInfo();
    
    //auto dev = app->getDefaultPhysicalDevice();

    //Device dev(instance);
    return 0;
}


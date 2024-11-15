#include <cstdint>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vector>
#include <iostream>
#include "instance.h"


class VulkanLayerAndExtension {
    public:
        VkResult getInstanceLayerProperties();
        VkResult getExtensionProperties(
                LayerProperties& layerProps,
                VkPhysicalDevice *gpu = NULL
                );

        VkResult getDeviceExtensionProperties(VkPhysicalDevice *gpu);
};

VkResult getExtensionProperties(
        LayerProperties& layerProps,
        VkPhysicalDevice *gpu = NULL
        ) {
    uint32_t extensionCount;
    VkResult result;

    char* layerName = layerProps.properties.layerName;
    vkEnumerateDeviceExtensionProperties(*gpu, layerName, &extensionCount, NULL);
}


VkResult VulkanLayerAndExtension::getInstanceLayerProperties() {
    uint32_t instanceLayerCount; 
    std::vector<VkLayerProperties> layerProperties;
    VkResult result;

    vkEnumerateInstanceLayerProperties(&instanceLayerCount, NULL);
    layerProperties.resize(instanceLayerCount);
    vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerProperties.data());

    for (auto globalLayerProp: layerProperties) {
        std::cout << std::endl <<
            globalLayerProp.layerName << std::endl << 
            "\t" << globalLayerProp.description << std::endl;

        LayerProperties layerProps;
        layerProps.properties = globalLayerProp;

        uint32_t propCount;
        std::vector<VkExtensionProperties> extensionProperties;

        vkEnumerateInstanceExtensionProperties(globalLayerProp.layerName, &propCount, nullptr);
        extensionProperties.resize(propCount);
        vkEnumerateInstanceExtensionProperties(globalLayerProp.layerName, &propCount, extensionProperties.data());
        for (auto extension: extensionProperties) {
            std::cout << "\t\t" << extension.extensionName << "(v" << extension.specVersion << ")"<< std::endl;
        }
    }
    return result;
}

int main (int argc, char *argv[]) {
    //VulkanLayerAndExtension a;
    //a.getInstanceLayerProperties();
    Instance i;

    i.getInstanceLayerProperties();
    i.init();
    i.print_info();

    VkInstance instance = i.getInstance();

    return 0;
}


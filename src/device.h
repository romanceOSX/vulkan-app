#include <string>
#include "vulkan/vulkan.h"
#include "vector"
#include <vulkan/vulkan_core.h>

void Device::getDeviceExtensions(VkPhysicalDevice *dev, Instance *instance) {

    /* Query global extensions first */
    uint32_t prop_count;
    std::vector<VkExtensionProperties> extensions;
    vkEnumerateDeviceExtensionProperties(*dev, nullptr, &prop_count, nullptr);
    extensions.resize(prop_count);
    vkEnumerateDeviceExtensionProperties(*dev, nullptr, &prop_count, extensions.data());

    for(auto extension: extensions) {
        std::cout << "+ " << extension.extensionName << " (v" << 
            extension.specVersion << ")" << std::endl;
    }

    /* Layer-specific device extensions */
    if (nullptr != instance) {
        for (auto layer: instance->m_layers) {
            uint32_t extension_count;
            std::vector<VkExtensionProperties> extensions;
            vkEnumerateDeviceExtensionProperties(*dev, layer.properties.layerName, &prop_count, nullptr);
            extensions.resize(prop_count);
            vkEnumerateDeviceExtensionProperties(*dev, nullptr, &prop_count, extensions.data());
            for (auto extension: extensions) {
                std::cout << "+ " << extension.extensionName << " (v" << 
                    extension.specVersion << ")" << std::endl;
            }
        }
    }
}

VkDevice Device::getDefaultDevice() {

}


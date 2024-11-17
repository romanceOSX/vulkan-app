#include <iostream>
#include <vulkan/vulkan_core.h>
#include "app.h"
#include "device.h"

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
    VkResult res;
    uint32_t devCount;
    std::vector<VkPhysicalDevice> devs;

    vkEnumeratePhysicalDevices(m_instance, &devCount, nullptr);
    devs.resize(devCount);
    vkEnumeratePhysicalDevices(m_instance, &devCount, devs.data());
    PRETTY_PRINT("Host's devices");
    std::cout << "Number of vulkan-capable devices: " << devCount << std::endl;
    for (auto& dev: devs) {
        VkPhysicalDeviceProperties devProps;
        vkGetPhysicalDeviceProperties(dev, &devProps);
        std::cout << "Device name: " << devProps.deviceName << std::endl
            << "Device Type: " << devProps.deviceType << std::endl;
        getDeviceQueueProperties(dev);
    }
}

void Device::getDeviceQueueProperties(VkPhysicalDevice& dev) {
    uint32_t propCount;
    std::vector<VkQueueFamilyProperties> props;

    vkGetPhysicalDeviceQueueFamilyProperties(dev, &propCount, nullptr);
    props.resize(propCount);
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &propCount, props.data());

    /* queue properties */
    PRETTY_PRINT("Physical device queue properties");
    for (auto& prop: props) {
        std::cout << "Queue Count: " << prop.queueCount << std::endl
            << "Queue Flags: " << prop.queueFlags << std::endl;
    }

    /* Physical Groups */
    getPhysicalDeviceGroups();

    /* queue Creation */
    VkDeviceQueueCreateInfo queue_create_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = _get_default_index(),
    };

    VkDeviceCreateInfo devCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    VkDevice device;

    if (VK_SUCCESS != vkCreateDevice(dev, &devCreateInfo, nullptr, &device)) {
        DBG_ERR("NOT SUCCESS!!");
    }
    PRETTY_PRINT("ALL GOOD");
}

void Device::getPhysicalDeviceGroups() {
    uint32_t devGroupsCount;
    std::vector<VkPhysicalDeviceGroupProperties> devGroups;

    vkEnumeratePhysicalDeviceGroups(m_instance, &devGroupsCount, devGroups.data());
    devGroups.resize(devGroupsCount);
    vkEnumeratePhysicalDeviceGroups(m_instance, &devGroupsCount, devGroups.data());
    
    PRETTY_PRINT("Device Groups");
    for (auto& devGroup: devGroups) {
        std::cout << "Device Count: " << devGroup.physicalDeviceCount << std::endl;
    }
}

uint32_t Device::_get_default_index(void) {
    return 0;
}

VkPhysicalDevice Device::_get_default_device(void) {
    return 0;
}

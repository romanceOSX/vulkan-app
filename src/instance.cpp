#include <iostream>
#include <string>
#include "instance.h"
#include "app.h"

#define VK_USE_PLATFORM_MACOS_MVK
#include "vulkan/vulkan.h"
#include <vulkan/vulkan_core.h>

VkResult Instance::getInstanceLayerProperties() {
    VkResult result;
    uint32_t instanceLayerCount; 
    std::vector<VkLayerProperties> _layers;

    /* get version */
    vkEnumerateInstanceVersion(&m_api_version);

    /* get instance properties */
    vkEnumerateInstanceLayerProperties(&instanceLayerCount, NULL);
    _layers.resize(instanceLayerCount);
    m_layers.resize(instanceLayerCount);
    vkEnumerateInstanceLayerProperties(&instanceLayerCount, _layers.data());

    /* query instance layers properties and save to custom structure */
    size_t i = 0;
    for (auto iter = _layers.begin(); iter != _layers.end(); iter++) {
        m_layers.at(i).properties = *iter;
        i++;
    }

    /* populate each layer's extensions if any */
    for (auto& layer: m_layers) {
        uint32_t propertyCount;
        vkEnumerateInstanceExtensionProperties(layer.properties.layerName, &propertyCount, nullptr);
        layer.extensions.resize(propertyCount);
        vkEnumerateInstanceExtensionProperties(layer.properties.layerName, &propertyCount, layer.extensions.data());
    }

    /* populate instance extensions (non-layer) */
    uint32_t instance_ext_count;
    vkEnumerateInstanceExtensionProperties(nullptr, &instance_ext_count, nullptr);
    m_instance_extensions.resize(instance_ext_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &instance_ext_count, m_instance_extensions.data());

    return result;
}

VkResult Instance::print_info() {
    PRETTY_PRINT("Vulkan API Version");
    std::cout << "v" << getInstanceVersion() << std::endl;

    PRETTY_PRINT("Instance Layers and extensions");
    for (auto layer: m_layers) {
        std::cout << "+ " << layer.properties.layerName << std::endl;
        for (auto extension: layer.extensions) {
            std::cout << "|--> " << extension.extensionName 
                << " (v" << extension.specVersion<< ")" << std::endl;
        }
    }
    PRETTY_PRINT("Instance Extensions");
    for (auto extension: m_instance_extensions) {
        std::cout << "+ " << extension.extensionName << " (v" <<
            extension.specVersion << ")" << std::endl;
    }
}

uint32_t Instance::getInstanceVersion() {
    return this->m_api_version;
}

VkResult Instance::createInstance(char_v& layers, char_v& extensions) {
    VkInstanceCreateInfo instance_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
        .pApplicationInfo = nullptr,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
    };

    instance_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instance_info.ppEnabledExtensionNames = extensions.data();

    instance_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
    instance_info.ppEnabledLayerNames = layers.data();

    VkResult result = vkCreateInstance(&instance_info, nullptr, &m_instance);

    if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
        DBG_ERR("Incompatible Driver!!!");
            return result;
    }
    else if (result != VK_SUCCESS) {
        DBG_ERR("Unknown Error");
            return result;
    }

    return result;
}

VkInstance Instance::getInstance() {
    return m_instance;
}

VkResult Instance::init() {
    VkResult res;
    std::vector<const char*> extensions {
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
    };

    std::vector<const char*> layers {
        "VK_LAYER_LUNARG_api_dump",
    };

    res = createInstance(layers, extensions);

    return res;
}


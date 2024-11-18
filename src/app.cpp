#include <iostream>

#include "app.h"
#include "instance.h"

App::App() {
    /* Populate available host layers and extensions */
    _getHostProperties();
}

App* App::getInstance() {
    if (_m_instance == nullptr) {
        _m_instance = new App();
    }   
    return _m_instance;
}

Instance* App::getVkInstance() {
    if (_m_vk_instance == nullptr) {
        _m_vk_instance = new Instance();
    }   
    return _m_vk_instance;

}

void App::printHostInfo() {
    PRETTY_PRINT("Vulkan API Version");
    std::cout << "v" << getVkInstanceVersion() << std::endl;

    PRETTY_PRINT("Instance Layers and extensions");
    for (auto layer: _m_layers) {
        std::cout << "+ " << layer.properties.layerName << std::endl;
        for (auto extension: layer.extensions) {
            std::cout << "|--> " << extension.extensionName 
                << " (v" << extension.specVersion<< ")" << std::endl;
        }
    }
    PRETTY_PRINT("Instance Extensions");
    for (auto extension: _m_instance_extensions) {
        std::cout << "+ " << extension.extensionName << " (v" <<
            extension.specVersion << ")" << std::endl;
    }
}

VkResult App::_getHostProperties() {
    VkResult result;
    uint32_t instanceLayerCount; 
    std::vector<VkLayerProperties> _layers;

    /* get version */
    vkEnumerateInstanceVersion(&_m_vk_api_version);

    /* get instance properties */
    vkEnumerateInstanceLayerProperties(&instanceLayerCount, NULL);
    _layers.resize(instanceLayerCount);
    _m_layers.resize(instanceLayerCount);
    vkEnumerateInstanceLayerProperties(&instanceLayerCount, _layers.data());

    /* query instance layers properties and save to custom structure */
    size_t i = 0;
    for (auto iter = _layers.begin(); iter != _layers.end(); iter++) {
        _m_layers.at(i).properties = *iter;
        i++;
    }

    /* populate each layer's extensions if any */
    for (auto& layer: _m_layers) {
        uint32_t propertyCount;
        vkEnumerateInstanceExtensionProperties(layer.properties.layerName, &propertyCount, nullptr);
        layer.extensions.resize(propertyCount);
        vkEnumerateInstanceExtensionProperties(layer.properties.layerName, &propertyCount, layer.extensions.data());
    }

    /* populate instance extensions (non-layer) */
    uint32_t instance_ext_count;
    vkEnumerateInstanceExtensionProperties(nullptr, &instance_ext_count, nullptr);
    _m_instance_extensions.resize(instance_ext_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &instance_ext_count, _m_instance_extensions.data());

    return result;
}

uint32_t App::getVkInstanceVersion(void) {
    return _m_vk_api_version;
}


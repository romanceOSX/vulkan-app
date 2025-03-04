#include <iostream>

#include "host.h"
#include "instance.h"
#include "device.h"
#include "physical_device.h"

/* static data members */
std::vector<VkPhysicalDevice> Host::_m_vk_physical_devs{};
std::vector<PhysicalDevice> Host::_m_physical_devs{};

Host::Host() {
    /* Populate available host layers and extensions */
    _getHostInstanceProperties();
}

Host::~Host() {
    /* Populate available host layers and extensions */
    PRETTY_PRINT("Destroying APP");
    delete _m_vk_instance;
}

Host* Host::getInstance() {
    if (_m_instance == nullptr) {
        _m_instance = new Host();
    }   
    return _m_instance;
}

Instance* Host::getVkInstance() {
    if (_m_vk_instance == nullptr) {
        _m_vk_instance = new Instance();
    }   
    return _m_vk_instance;
}

void Host::printHostInfo() {
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

    PRETTY_PRINT("Available Physical Devices");
    /* Physical Device information is only available after Instance init */
    if (getVkInstance()->isInit()) {
        if (!_m_isVkInstanceInit) {
            _getHostPhysicalDevices();
            _m_isVkInstanceInit = true;
        }

        std::cout << "Number of Vulkan-Capable Devices: " <<
            _m_physical_devs.size() << std::endl;

        for (auto& dev: _m_physical_devs) {
            PRETTY_PRINT_CUSTOM("Physical Device", "🥫");
            auto devProps = dev.getDeviceProperties();
            std::cout << "Device Name: " << devProps.deviceName << std::endl
                << "Device Type: " << devProps.deviceType << std::endl;

            PRETTY_PRINT_CUSTOM("Device Available Extensions", "🐝");
            auto extensions = dev.getDeviceExtensions();
            for (auto& ext: extensions) {
                std::cout << "--> " << ext.extensionName 
                    << " (v" << ext.specVersion << ")" << std::endl;
            }

            PRETTY_PRINT_CUSTOM("Queue Information", "💧");
            std::cout << "Available Queues: " << dev.getQueueCount() << std::endl;

            for (auto& queue: dev.getDeviceQueueProperties()) {
                std::cout << "|---- Queue Flags: " << queue.queueFlags << std::endl
                    << "      Queue Count: " << queue.queueCount << std::endl;
            }

            PRETTY_PRINT("Device Groups");
            for (auto& devGroup: _m_physical_dev_groups) {
                std::cout << "Device Count: " << devGroup.physicalDeviceCount << std::endl;
            }
        }
    }
    else {
        std::cout << "Physical Device Information not available, Initialize VkInstance first";
    }
}

VkResult Host::_getHostPhysicalDevices() {
    /* get physical devices */
    VkResult res;
    uint32_t devCount;

    VkInstance instance = getVkInstance()->getInstance();

    vkEnumeratePhysicalDevices(instance, &devCount, nullptr);
    _m_vk_physical_devs.resize(devCount);
    vkEnumeratePhysicalDevices(instance, &devCount, _m_vk_physical_devs.data());

    //PRETTY_PRINT("Host's devices");
    for (auto& dev: _m_vk_physical_devs) {
        _m_physical_devs.emplace_back(dev);
    }

    /* get physical device groups */
    uint32_t devGroupsCount;

    vkEnumeratePhysicalDeviceGroups(instance, &devGroupsCount, nullptr);
    _m_physical_dev_groups.resize(devGroupsCount);
    vkEnumeratePhysicalDeviceGroups(instance, &devGroupsCount, _m_physical_dev_groups.data());
}

VkResult Host::_getHostInstanceProperties() {
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

uint32_t Host::getVkInstanceVersion(void) {
    return _m_vk_api_version;
}

PhysicalDevice& Host::getDefaultDevice(void) {
    return  _m_physical_devs.front();
}


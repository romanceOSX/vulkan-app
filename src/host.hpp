#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <vulkan/vulkan_core.h>

#include <iostream>

/*
 * Forward declarations
 */
class Instance;
class LayerProperties;
class PhysicalDevice;

// TODO: Implement query of physical device groups
/*
 * Representation of the host
 */
class Host {
    public:
        Host(Host& other) = delete;
        void operator=(const Host&) = delete;
        static Host* getInstance();
        Instance* getVkInstance();
        void printHostInfo();
        uint32_t getVkInstanceVersion(void);
        /*
         * Vulkan Instance must be initialized already
         * TODO: add a check for the vk instance validity 
         */
        static PhysicalDevice& getDefaultDevice(void);
        ~Host();

        std::vector<VkPhysicalDevice> m_phy_devs;

    private:
        Host();
        VkResult _getHostInstanceProperties();
        VkResult _getHostPhysicalDevices();

        inline static Host*                                 _m_instance = nullptr;
        Instance*                                           _m_vk_instance = nullptr;
        std::vector<LayerProperties>                        _m_layers;
        std::vector<VkExtensionProperties>                  _m_instance_extensions;
        static std::vector<VkPhysicalDevice>                _m_vk_physical_devs;
        static std::vector<PhysicalDevice>                  _m_physical_devs;
        std::vector<VkPhysicalDeviceGroupProperties>        _m_physical_dev_groups;
        uint32_t                                            _m_vk_api_version;
        bool                                                _m_isVkInstanceInit = false;
};

enum class AppResult {
    APP_SUCCESS = 0,
    APP_ERR = 0,
};


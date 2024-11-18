#pragma once

#include <vulkan/vulkan.h>
#include <vector>

/* 
 * Utility Macros
 */
#define PRETTY_PRINT(str) std::cout << "🎆 ~~~ " << str << " ~~~ 🎆" << std::endl
#define DBG_ERR(msg) std::cout << "❌❌ " msg << std::endl
#define PRETTY_PRINT_CUSTOM(msg, symbol) std::cout << #symbol " ~~~ " << msg << " ~~~ " #symbol << std::endl

/*
 * Forward declarations
 */
class Instance;
class LayerProperties;
class PhysicalDevice;

/*
 * Representation of the host
 */
class App {
    public:
        App(App& other) = delete;
        void operator=(const App&) = delete;
        static App* getInstance();
        Instance* getVkInstance();
        VkPhysicalDevice getDefaultPhysicalDevice(void);
        void printHostInfo();
        uint32_t getVkInstanceVersion(void);
        ~App();

        std::vector<VkPhysicalDevice> m_phy_devs;

    private:
        App();
        VkResult _getHostInstanceProperties();
        VkResult _getHostPhysicalDevices();

        inline static App*                      _m_instance = nullptr;
        Instance*                               _m_vk_instance = nullptr;
        std::vector<LayerProperties>            _m_layers;
        std::vector<VkExtensionProperties>      _m_instance_extensions;
        std::vector<VkPhysicalDevice>           _m_vk_physical_devs;
        std::vector<PhysicalDevice>             _m_physical_devs;
        std::vector<PhysicalDevice>             _m_physical_dev_groups;
        uint32_t                                _m_vk_api_version;
        bool                                    _m_isVkInstanceInit = false;
};

enum class AppResult {
    APP_SUCCESS = 0,
    APP_ERR = 0,
};


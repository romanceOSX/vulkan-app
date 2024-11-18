#pragma once

#include <vulkan/vulkan.h>
#include <vector>

/* 
 * Utility Macros
 */
#define PRETTY_PRINT(str) std::cout << "🎆 ~~~ " << str << " ~~~ 🎆" << std::endl
#define DBG_ERR(msg) std::cout << "❌❌ " msg << std::endl

/*
 * Forward declarations
 */
class Instance;
class LayerProperties;

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
        VkResult _getHostProperties();

        inline static App* _m_instance = nullptr;
        Instance* _m_vk_instance;
        std::vector<LayerProperties> _m_layers;
        std::vector<VkExtensionProperties> _m_instance_extensions;
        uint32_t _m_vk_api_version;
};

enum class AppResult {
    APP_SUCCESS = 0,
    APP_ERR = 0,
};


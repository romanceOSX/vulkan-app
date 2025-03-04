#include "app_settings.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class VulkanApp {
    public: 
        static VulkanApp* getInstance();
        void run();
        void run_stub();
        ~VulkanApp();
            
    private:
        VulkanApp() = default; 
        void _initWindow();
        void _initVulkan();
        void _mainLoop();
        void _cleanup();

        inline static VulkanApp*    _m_app_instance = nullptr;
        GLFWwindow* _m_window                       = nullptr;
};


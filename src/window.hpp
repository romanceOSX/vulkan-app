
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include "instance.h"
#include "host.h"
#include "device.h"

class Window {
    public:
        Window(Instance& instance);
        Window(Instance& instance, GLFWwindow* window);
        Window(Window& other) = delete;
        ~Window();
        bool is_device_suitable(Device& dev);
        void wait_to_close_window();
        VkSurfaceKHR get_vk_surface();

    private:
        VkSurfaceKHR        m_vk_surface;
        Instance&           m_instance;
        GLFWwindow*         m_glfw_window;
};


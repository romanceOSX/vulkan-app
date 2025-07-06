#pragma once

#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include "instance.hpp"
#include "host.hpp"
#include "device.hpp"

class Window {
    public:
        Window(Instance& instance);
        Window(Instance& instance, GLFWwindow* window);
        Window(Window& other) = delete;
        ~Window();
        void wait_to_close_window();
        VkSurfaceKHR get_vk_surface();
        GLFWwindow* get_glfw_window();

    private:
        VkSurfaceKHR        m_vk_surface;
        Instance&           m_instance;
        GLFWwindow*         m_glfw_window;
};


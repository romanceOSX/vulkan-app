
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "app_settings.h"
#include "window.hpp"

Window::Window(Instance& i): m_instance{i} {
    /* Initialize glfw */
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#ifndef APP_RESIZABLE_WINDOW
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#endif /* !APP_RESIZABLE_WINDOW */
    m_glfw_window = glfwCreateWindow(APP_WINDOW_WIDTH, APP_WINDOW_HEIGHT, APP_WINDOW_NAME, nullptr, nullptr);

    /* TODO: window surface creation does not work out of the box */
    /* Create vk surface */
    //if (VK_SUCCESS != glfwCreateWindowSurface(m_instance.get_vk_instance(), m_glfw_window, nullptr, &m_surface)) {
    //    throw std::runtime_error("Failed to create window surface ❌");
    //}
}

/* Stub constructor */
Window::Window(Instance& instance, GLFWwindow* window): m_instance{instance}, m_glfw_window{window} {
    /* Create vk surface */
    if (VK_SUCCESS != glfwCreateWindowSurface(m_instance.get_vk_instance(), m_glfw_window, nullptr, &m_vk_surface)) {
        throw std::runtime_error("Failed to create window surface ❌");
    }
}

Window::~Window() {
    vkDestroySurfaceKHR(m_instance.get_vk_instance(), m_vk_surface, nullptr);
    /* TODO: should I destroy vkInstance here? */
    //vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks *pAllocator)
}

bool Window::is_device_suitable(Device &dev) {
    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR(
            dev.get_vk_physical_dev(),
            dev.getQueueFamilyIndex(),
            m_vk_surface,
            &res
    );
    return res;
}

void Window::wait_to_close_window() {
    while (!glfwWindowShouldClose(m_glfw_window)) {
        glfwPollEvents();
    }
}

VkSurfaceKHR Window::get_vk_surface() {
    return m_vk_surface; 
}


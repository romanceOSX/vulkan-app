/*
 * Vulkan-hpp sample test
 */

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <print>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>
#include <ranges>
#include <algorithm>
#include <utility>

#define VK_ENABLE_BETA_EXTENSIONS
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VK_USE_PLATFORM_METAL_EXT
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <vulkan/vulkan_enums.hpp>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "utils/utils.hpp"
#include "utils/ostream_formatters.hpp"

/* 
 * namespace imports
 */
namespace views = std::views;
namespace ranges = std::ranges;

using std::vector;
using std::string;

constexpr string AppName    = "01_InitInstanceRAII";
constexpr string EngineName = "Vulkan.hpp";

/*
 * utilities
 */
namespace vu {

uint32_t findGraphicsQueueFamilyIndex(vk::raii::PhysicalDevice& phy_dev) {
    auto queue_families = phy_dev.getQueueFamilyProperties();

    auto graphics_queue = ranges::find_if(queue_families, [](vk::QueueFamilyProperties& queue){
        return static_cast<bool>(queue.queueFlags & vk::QueueFlagBits::eGraphics);
    });

    assert(graphics_queue != queue_families.end());

    std::cout << "--Found graphics queue!! " << *graphics_queue << std::endl;

    return static_cast<uint32_t>(std::distance(queue_families.begin(), graphics_queue));
}

// tuple that represents a physical device and a queue index
using QueuePhyDeviceTup_t = std::tuple<vk::raii::PhysicalDevice, uint32_t>;

// finds the best suitable physical device, and queue_family
QueuePhyDeviceTup_t GetSuitableDevice(vk::raii::Instance& instance, vk::raii::SurfaceKHR& surface) {
    for (auto& phy_dev : instance.enumeratePhysicalDevices()) {
        auto queues = phy_dev.getQueueFamilyProperties();
        for (auto it = queues.begin(); it != queues.end(); ++it) {
            uint32_t index = static_cast<uint32_t>(std::distance(queues.begin(), it));
            if (phy_dev.getSurfaceSupportKHR(index, surface))
                assert(it != queues.end());
                return std::make_tuple(phy_dev, index);
        }
    }
}

vk::raii::SurfaceKHR CreateWindowSurface(vk::raii::Instance& instance) {
    uint32_t width = 500;
    uint32_t height = 500;
    
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    VkSurfaceKHR _surface;
    GLFWwindow* window = glfwCreateWindow(width, height, "vulkan hpp", nullptr, nullptr);

    assert(window != nullptr);

    if (VK_SUCCESS != glfwCreateWindowSurface(*instance, window, nullptr, &_surface)) {
        throw std::runtime_error("Failed to create window surface ❌");
    }

    return vk::raii::SurfaceKHR(instance, _surface);
}

// create swapchain
void CreateSwapchain(vk::raii::PhysicalDevice& phy_dev, vk::raii::SurfaceKHR& surface) {
}

// get present and graphics queue families
//

}

void printVulkanPlatformInfo(vk::raii::Context &ctx) {
    std::cout << std::format("Vulkan v{}", ctx.enumerateInstanceVersion()) << std::endl;
    std::cout << "Available instance layers:" << std::endl;
    std::cout << ctx.enumerateInstanceLayerProperties();
    std::cout << "Available instance extensions:" << std::endl;
    std::cout << ctx.enumerateInstanceExtensionProperties();
}

void printInstanceInfo(std::unique_ptr<vk::raii::Instance>& instance) {
    auto vec = instance->enumeratePhysicalDevices();

    /* print devices */
    ranges::for_each(vec, [](vk::raii::PhysicalDevice& dev) {
        std::cout << "- Device " << std::endl
            << std::format("Queue Families ({}):", dev.getQueueFamilyProperties().size()) << std::endl;
        ranges::for_each(dev.getQueueFamilyProperties(), [](auto& queue) {
            std::cout << "\t- " << queue << std::endl;
        });
    });

    /* find suitable index? */

    //utils_print_container(vec);
}

void testVulkan() {
    std::cout << "---Vulkan sample App :3---" << std::endl;

    vk::raii::Context ctx;

    printVulkanPlatformInfo(ctx);

    // Instance creation
    vector<const char*> layers{
        "VK_LAYER_KHRONOS_validation",
    };

    vector<const char*> extensions{
        vk::KHRGetPhysicalDeviceProperties2ExtensionName,
        vk::KHRPortabilityEnumerationExtensionName,
        vk::KHRSurfaceExtensionName,
        vk::EXTMetalSurfaceExtensionName,
    };

    vk::InstanceCreateInfo instance_create{
        .flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    auto instance = std::make_unique<vk::raii::Instance>(ctx, instance_create);

    printInstanceInfo(instance);

    // Window Creation
    vk::raii::SurfaceKHR surface = vu::CreateWindowSurface(*instance);

    // Device Creation
    // NOTE: what is the point of raii'ing a physical device if its creation depends on instance?
    //vk::raii::PhysicalDevice phy_dev = instance->enumeratePhysicalDevices().front();

    // find device by graphics
    //auto queue_index = vu::findGraphicsQueueFamilyIndex(phy_dev);
    
    // find device by window-compatibility
    auto[phy_dev, queue_index] = vu::GetSuitableDevice(*instance, surface);

    std::cout << std::format("--Queue index: {}", queue_index) << std::endl;

    float queue_priority = 0.0f;

    vector<const char*> dev_extensions{
        vk::KHRPortabilitySubsetExtensionName,
    };

    vk::DeviceQueueCreateInfo queue_create {
        .queueFamilyIndex = queue_index,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };
    vk::DeviceCreateInfo dev_create {
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_create,
        .enabledExtensionCount = static_cast<uint32_t>(dev_extensions.size()),
        .ppEnabledExtensionNames = dev_extensions.data(),
    };

    vk::raii::Device device{phy_dev, dev_create};

    // Command Pool
    vk::CommandPoolCreateInfo command_pool_create {
        .queueFamilyIndex = queue_index,
    };
    vk::raii::CommandPool command_pool(device, command_pool_create);
    
    // Command Buffer
    vk::CommandBufferAllocateInfo cmd_buf_alloc_info {
        .commandPool = command_pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1,
    };
    // NOTE: CommandBuffers inherits from the vector class
    vk::raii::CommandBuffer command_buffer = std::move(vk::raii::CommandBuffers(device, cmd_buf_alloc_info).front());

    // Swapchain Creation

    // get capabilities
    vk::SurfaceCapabilitiesKHR capabilities = phy_dev.getSurfaceCapabilitiesKHR(surface);
    
    // get supported formats
    vector<vk::SurfaceFormatKHR> formats = phy_dev.getSurfaceFormatsKHR(surface);
    assert(!formats.empty());

    // get present modes
    vector<vk::PresentModeKHR> surface_present_modes = phy_dev.getSurfacePresentModesKHR();

    vk::SwapchainCreateInfoKHR swapchain_create{
    };

    //ut::print_container(formats);
}

void testVulkanUtilsQueueFamilies(vk::raii::Instance& instance) {
    auto device = instance.enumeratePhysicalDevices().front();
    vector<uint32_t> graphics_queue_families = vu::GetGraphicsQueueFamilyIndexes(device);
    ut::print_container(graphics_queue_families);
}

void testVulkanUtils() {
    std::cout << "--- Testing Vulkan Utils" << std::endl; 
    vk::raii::Context ctx{};
    
    // Instance creation
    vector<const char*> layers{
        "VK_LAYER_KHRONOS_validation",
    };

    vector<const char*> extensions{
        vk::KHRGetPhysicalDeviceProperties2ExtensionName,
        vk::KHRPortabilityEnumerationExtensionName,
        vk::KHRSurfaceExtensionName,
        vk::EXTMetalSurfaceExtensionName,
    };

    vk::InstanceCreateInfo instance_create{
        .flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    auto instance = std::make_unique<vk::raii::Instance>(ctx, instance_create);

    testVulkanUtilsQueueFamilies(*instance);
}

int main( int /*argc*/, char ** /*argv*/ )
{
    testVulkan();
    testVulkanUtils();
}


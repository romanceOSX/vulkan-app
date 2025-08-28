/*
 * Vulkan-hpp sample test
 */

#include <algorithm>
#include <iostream>
#include <memory>
#include <print>
#include <sstream>
#include <utility>
#include <vector>
#include <ranges>
#include <algorithm>
#include <utility>

#define VK_ENABLE_BETA_EXTENSIONS
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VK_USE_PLATFORM_METAL_EXT
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <vulkan/vulkan_enums.hpp>

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

    vector<const char*> layers{
        "VK_LAYER_KHRONOS_validation",
    };

    vector<const char*> extensions{
        vk::KHRGetPhysicalDeviceProperties2ExtensionName,
        vk::KHRPortabilityEnumerationExtensionName,
        //vk::EXTMetalSurfaceExtensionName,
        //vk::KHRSurfaceExtensionName,
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

    /* NOTE: what is the point of raii'ing a physical device if its creation depends on instance? */
    vk::raii::PhysicalDevice phy_dev = instance->enumeratePhysicalDevices().front();

    auto queue_index = vu::findGraphicsQueueFamilyIndex(phy_dev);

    std::cout << std::format("--Queue index={}", queue_index) << std::endl;

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

    vk::raii::Device dev{phy_dev, dev_create};
}

int main( int /*argc*/, char ** /*argv*/ )
{
    testVulkan();
}


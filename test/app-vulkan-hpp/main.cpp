//
// Vulkan-hpp sample test
//

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

// 
// namespace imports
// 
namespace views = std::views;
namespace ranges = std::ranges;

using std::vector;
using std::string;

constexpr string AppName    = "01_InitInstanceRAII";
constexpr string EngineName = "Vulkan.hpp";

//
// utils?
//
// TODO: move this to the utils namespace or ostream formatters?
void printVulkanPlatformInfo(vk::raii::Context &ctx) {
    std::cout << std::format("Vulkan v{}", ctx.enumerateInstanceVersion()) << std::endl;
    std::cout << "Available instance layers:" << std::endl;
    std::cout << ctx.enumerateInstanceLayerProperties();
    std::cout << "Available instance extensions:" << std::endl;
    std::cout << ctx.enumerateInstanceExtensionProperties();
}

void printInstanceInfo(std::unique_ptr<vk::raii::Instance>& instance) {
    auto vec = instance->enumeratePhysicalDevices();

    // print devices
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

// 
// Test Vulkan app
// 
// TODO: either delete this or make it useful
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
    vk::raii::SurfaceKHR surface = vu::createWindowSurface(*instance);

    // Device Creation
    // NOTE: what is the point of raii'ing a physical device if its creation depends on instance?
    //vk::raii::PhysicalDevice phy_dev = instance->enumeratePhysicalDevices().front();

    // find device by graphics
    //auto queue_index = vu::findGraphicsQueueFamilyIndex(phy_dev);
    
    // find device by window-compatibility
    auto[phy_dev, queue_index] = vu::getSuitableDevice(*instance, surface);

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

    // get all graphics queues
    vector<uint32_t> graphics_queue_families = vu::getGraphicsQueueFamilyIndexes(device);
    ut::printContainer(graphics_queue_families);

    // get a single grpahics queue
    uint32_t graphics_index = vu::getGraphicsQueueFamilyIndex(device);

    // get all present queues
    vk::raii::SurfaceKHR surface = vu::createWindowSurface(instance);
    vector<uint32_t> present_queue_families = vu::getPresentationFamilyIndexes(device, surface);
    ut::printContainer(present_queue_families);

    // get a single present queue
    uint32_t present_index = vu::getPresentationFamilyIndex(device, surface);

    // get transfer queue
    uint32_t transfer_queue = vu::getQueueFamilyIndex(device, vk::QueueFlagBits::eTransfer);

    std::cout << std::format("Graphics index = {}\n", graphics_index);
    std::cout << std::format("Presentation index = {}\n", present_index);
    std::cout << std::format("Transfer index = {}\n", present_index);
}

//
// Testing Vulkan Utils library
//
void testVulkanUtils() {
    ut::prettyPrint("Testing Vulkan Utils 👀");
    vk::raii::Context ctx{};
    
    //
    // Instance creation
    //
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
    auto phy_dev = instance->enumeratePhysicalDevices().front();

    //
    // test
    //
    testVulkanUtilsQueueFamilies(*instance);

    //
    // Surface creation
    //
    vk::raii::SurfaceKHR surface = vu::createWindowSurface(*instance);

    vk::SurfaceCapabilitiesKHR surface_capabilities = phy_dev.getSurfaceCapabilitiesKHR(surface);

    // 
    // Device creation
    // 
    vector<const char*> dev_extensions {
        vk::KHRSwapchainExtensionName,
        vk::KHRPortabilitySubsetExtensionName,
    };

    uint32_t graphics_index = vu::getGraphicsQueueFamilyIndex(phy_dev);

    //
    // Queue Creation
    //
    float queue_priority = 1.0f;

    vk::DeviceQueueCreateInfo queue_create {
        .queueFamilyIndex = graphics_index,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };

    vk::DeviceCreateInfo dev_create {
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_create,
        .enabledExtensionCount = static_cast<uint32_t>(dev_extensions.size()),
        .ppEnabledExtensionNames = dev_extensions.data(),
    };

    auto device = phy_dev.createDevice(dev_create);

    //
    // Swapchain Creation 
    //
    SurfaceProperties surface_properties = vu::getSurfaceProperties(phy_dev, surface);

    // NOTE: learn what each of these components represent
    vk::SwapchainCreateInfoKHR swapchain_create {
        .flags = vk::SwapchainCreateFlagsKHR(),
        .surface = surface,
        .minImageCount = surface_properties.capabilities.minImageCount,
        .imageFormat = surface_properties.formats.front().format,
        .imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
        .imageExtent = surface_properties.capabilities.currentExtent,
        .imageArrayLayers = 1, 
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = surface_properties.capabilities.currentTransform,
        .presentMode = vk::PresentModeKHR::eFifo,
        .oldSwapchain = nullptr,
    };

    vk::raii::SwapchainKHR swapchain = device.createSwapchainKHR(swapchain_create);
    std::cout << "Swapchain created succesfully!" << std::endl;

    //
    // Command Pool
    //
    vk::CommandPoolCreateInfo command_pool_create {
        //.flags = vk::CommandPoolCreateFlagBits::eTransient,        TODO: what usage should I set here?
        .queueFamilyIndex = graphics_index,
    };

    vk::raii::CommandPool command_pool = device.createCommandPool(command_pool_create);

    //
    // Command Buffer
    //
    vk::CommandBufferAllocateInfo command_buffer_alloc {
        .commandPool = command_pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1,
    };

    vector<vk::raii::CommandBuffer> command_buffers = device.allocateCommandBuffers(command_buffer_alloc);
    std::cout << "Created command buffers!" << std::endl;

    //
    // Image Views
    //
    // Create one image view per each of the Swapchain's VkImage's
    vector<vk::Image> images = swapchain.getImages();
    vector<vk::raii::ImageView> image_views;

    for (auto& image: images) {
       // create image view
        vk::ImageViewCreateInfo image_view_create {
            .image = image,
            .viewType = vk::ImageViewType::e2D,
            .format = vu::chooseSurfaceFormat(surface_properties.formats),
        };

        vk::raii::ImageView image_view = device.createImageView(image_view_create);
        image_views.push_back(std::move(image_view));
    }

    // TODO: make a stream printer for image views
    //ut::printContainer(image_views);
}

//
// plain main
//
int main( int /*argc*/, char ** /*argv*/ )
{
    testVulkanUtils();
    //testVulkan();
}


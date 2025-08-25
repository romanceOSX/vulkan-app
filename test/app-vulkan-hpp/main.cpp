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

#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VK_USE_PLATFORM_METAL_EXT
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <vulkan/vulkan_enums.hpp>

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
 * program utils
 */
template <ranges::range T>
void utils_print_container(T& cont) {
    ranges::for_each(cont, [](auto& e) { std::cout << "**" << e << std::endl; });
}

/* 
 * ostream overrides
 */
std::ostream& operator<<(std::ostream& stream, vector<vk::LayerProperties> layers) {
    for (auto& layer: layers) {
        stream
            << "\t- "
            << layer.layerName << " (" << layer.description << ")" << std::endl
        ;
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, vector<vk::ExtensionProperties> extensions) {
    for (auto& ext: extensions) {
        stream
            << "\t- "
            << ext.extensionName << " (v" << ext.specVersion << ")" << std::endl
        ;
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, vk::QueueFamilyProperties& queue) {
    stream << std::format(
            "Queue Family flags: 0x{:02x}, count: {}",
            static_cast<vk::QueueFlags::MaskType>(queue.queueFlags),
            queue.queueCount
        );
    return stream;
}

std::ostream& operator<<(std::ostream& stream, vk::raii::PhysicalDevice& dev) {
    auto queue_props = dev.getQueueFamilyProperties();
    ranges::for_each(queue_props, [&](auto &q) { stream << 2;} );
    return stream;
}

void print_vulkan_platform_info(vk::raii::Context &ctx) {
    std::cout << "Querying instance layers..." << std::endl;
    std::cout << ctx.enumerateInstanceLayerProperties();
    std::cout << "Querying extensions..." << std::endl;
    std::cout << ctx.enumerateInstanceExtensionProperties();
}

void print_instance_info(std::unique_ptr<vk::raii::Instance>& instance) {
    auto vec = instance->enumeratePhysicalDevices();
    ranges::for_each(vec, [](vk::raii::PhysicalDevice& dev) {
        std::cout << "- Device " << std::endl
            << std::format("Queue Families ({}):", dev.getQueueFamilyProperties().size()) << std::endl;
        ranges::for_each(dev.getQueueFamilyProperties(), [](auto& queue) {
            std::cout << "\t- " << queue << std::endl;
        });
    });

    //utils_print_container(vec);
}

int test_template() {
    /* VULKAN_HPP_KEY_START */
    try
    {
        /* 
         * The vulkan context is in charge of acting as a dispatcher for the vulkan loader
         *
         * The dispatcher is in charge of getting the correct function to call out of the
         * vulkan loader at runtime
         *  --> https://github.com/KhronosGroup/Vulkan-Loader
         */
        vk::raii::Context context;

        print_vulkan_platform_info(context);

        /* get instance layer properties */
        std::vector<vk::LayerProperties> layers = context.enumerateInstanceLayerProperties();

        for (auto& layer: layers) {
            std::cout << layer.layerName << "\t" << layer.description << std::endl;
        }

        /* fill-in app info */
        vk::ApplicationInfo app_info{
            .pApplicationName = AppName.c_str(),  
        };

        //vk::InstanceCreateInfo instance_create_info({}, &app_info);
        vk::InstanceCreateInfo instance_create_info{
            .pApplicationInfo = &app_info,
        };

        /* create instance */
        auto instance = std::make_unique<vk::raii::Instance>(context, instance_create_info);
    }
    catch ( vk::SystemError & err )
    {
        std::cout << "vk::SystemError: " << err.what() << std::endl;
        exit( -1 );
    }
    catch ( std::exception & err )
    {
        std::cout << "std::exception: " << err.what() << std::endl;
        exit( -1 );
    }
    catch ( ... )
    {
        std::cout << "unknown error\n";
        exit( -1 );
    }

    /* VULKAN_HPP_KEY_END */

    return 0;
}

void test_misc_api() {
}

void test_vulkan() {
    std::cout << "Vulkan application :3" << std::endl;

    vk::raii::Context ctx;
    print_vulkan_platform_info(ctx);

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

    print_instance_info(instance);

    //ctx.createInstance(const vk::InstanceCreateInfo &createInfo)
    //vk::createInstance()
}

int main( int /*argc*/, char ** /*argv*/ )
{
    test_vulkan();
    //test_template();
}


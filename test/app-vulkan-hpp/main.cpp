/*
 * Vulkan-hpp sample test
 */

#include <algorithm>
#include <iostream>
#include <memory>
#include <print>
#include <sstream>
#include <vector>
#include <ranges>
#include <algorithm>

#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VK_USE_PLATFORM_METAL_EXT
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_handles.hpp>

/* 
 * namespace imports
 */
using std::vector;
using std::string;

static string AppName    = "01_InitInstanceRAII";
static string EngineName = "Vulkan.hpp";

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

void print_system_info(vk::raii::Context &ctx) {
    std::cout << "Querying instance layers..." << std::endl;
    std::cout << ctx.enumerateInstanceLayerProperties();
    std::cout << "Querying extensions..." << std::endl;
    std::cout << ctx.enumerateInstanceExtensionProperties();
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

        print_system_info(context);

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

namespace views = std::views;
namespace ranges = std::ranges;

void test_vulkan() {
    std::cout << "Vulkan application :3" << std::endl;

    vk::raii::Context ctx;
    print_system_info(ctx);

    vector<const char*> layers{
        "VK_LAYER_KHRONOS_validation",
    };

    vector<const char*> extensions{
        vk::KHRGetPhysicalDeviceProperties2ExtensionName,
        vk::KHRPortabilityEnumerationExtensionName,
        //vk::EXTMetalSurfaceExtensionName,
        //vk::KHRSurfaceExtensionName,
    };

    ranges::for_each(extensions, [](auto& s) {std::cout << "**" << s << std::endl;});

    vk::InstanceCreateInfo instance_create{
        .flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    auto instance = std::make_unique<vk::raii::Instance>(ctx, instance_create);

    //ctx.createInstance(const vk::InstanceCreateInfo &createInfo)
    //vk::createInstance()
}

int main( int /*argc*/, char ** /*argv*/ )
{
    test_vulkan();
    //test_template();
}


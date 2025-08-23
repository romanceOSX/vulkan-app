/*
 * Vulkan-hpp sample test
 */

#include <iostream>
#include <memory>
#include <print>
#include <sstream>
#include <vector>

#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

static std::string AppName    = "01_InitInstanceRAII";
static std::string EngineName = "Vulkan.hpp";

using std::vector;

void print_available_instance_layers(vk::raii::Context &ctx) {
    vector<vk::LayerProperties> properties = ctx.enumerateInstanceLayerProperties();
    for (auto& p: properties) {
        std::cout 
            << p.layerName << std::endl
            << p.description << std::endl;
    }
}

std::ostream& operator<<(std::ostream& stream, vector<vk::LayerProperties> layers) {
    for (auto& layer: layers) {
        stream
            << "\t- "
            << layer.layerName << " (" << layer.description << ")" << std::endl
        ;
    }

    return stream;
}

void scan_system(vk::raii::Context &ctx) {
    std::cout << ctx.enumerateInstanceLayerProperties();

    //vector<vk::ExtensionProperties> extensions = ctx.enumerateInstanceExtensionProperties();
    //for (auto& ext: extensions) {
    //    std::cout
    //        << "\t- "
    //        << ext.extensionName << std::endl
    //        ;
    //}
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

        scan_system(context);

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

void test_vulkan() {
    vk::raii::Context ctx;
    scan_system(ctx);
}

int main( int /*argc*/, char ** /*argv*/ )
{
    test_vulkan();
    //test_template();
}


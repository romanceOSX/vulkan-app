
#include <algorithm>
#include <vector>
#include <ranges>

#include <vulkan/vulkan_raii.hpp>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "utils.hpp"

using std::vector;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

namespace ut {

void prettyPrint(const std::string& prompt) {
    std::cout << std::format("-- {}", prompt) << std::endl;
}
    
}

namespace vu {

// finds the graphics queue family
uint32_t findGraphicsQueueFamilyIndex(vk::raii::PhysicalDevice& phy_dev) {
    auto queue_families = phy_dev.getQueueFamilyProperties();

    auto graphics_queue = ranges::find_if(queue_families, [](vk::QueueFamilyProperties& queue){
        return static_cast<bool>(queue.queueFlags & vk::QueueFlagBits::eGraphics);
    });

    assert(graphics_queue != queue_families.end());

    std::cout << "--Found graphics queue!! " << *graphics_queue << std::endl;

    return static_cast<uint32_t>(std::distance(queue_families.begin(), graphics_queue));
}

// finds the best suitable physical device, and queue_family
QueuePhyDeviceTup_t getSuitableDevice(vk::raii::Instance& instance, vk::raii::SurfaceKHR& surface) {
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

// TODO: make this clearer
vk::raii::SurfaceKHR createWindowSurface(vk::raii::Instance& instance) {
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

// get graphic bit from a device
uint32_t getGraphicsQueueFamilyIndex(vk::raii::PhysicalDevice& phy_dev) {
    uint32_t res = 0;
    vector<vk::QueueFamilyProperties> properties = phy_dev.getQueueFamilyProperties();

    for (auto it = properties.begin(); it != properties.end(); ++it) {
        if (it->queueFlags & vk::QueueFlagBits::eGraphics) {
            res = static_cast<uint32_t>(std::distance(properties.begin(), it));
            break;
        }
    }

    return res;
}

// returns all the available queue families with the requested indexes
vector<uint32_t> getGraphicsQueueFamilyIndexes(vk::raii::PhysicalDevice& phy_dev) {
    vector<vk::QueueFamilyProperties> properties = phy_dev.getQueueFamilyProperties();
    size_t count = 0;

    auto indexes = views::iota(static_cast<size_t>(0), properties.size());
    
    auto res_indexes = indexes
        | views::filter([&](size_t index) {
            return static_cast<bool>((properties.at(index).queueFlags) & (vk::QueueFlagBits::eGraphics));
        })
        | ranges::to<vector<uint32_t>>();

    return res_indexes;
}

// graphic and presentation CAN be different
vector<uint32_t> getPresentationFamilyIndexes(vk::raii::PhysicalDevice& phy_dev, vk::raii::SurfaceKHR& surface) {
    auto queue_families = phy_dev.getQueueFamilyProperties();
    auto indexes = views::iota(static_cast<size_t>(0), queue_families.size());

    auto res_indexes = indexes
        | views::filter([&](size_t index) {     // NOTE: why can't I borrow by ref here
            return static_cast<bool>(phy_dev.getSurfaceSupportKHR(index, surface));
        })
        | ranges::to<std::vector<uint32_t>>();
    return res_indexes;
}

// gets a single present queue
uint32_t getPresentationFamilyIndex(vk::raii::PhysicalDevice& phy_dev, vk::raii::SurfaceKHR& surface) {
    auto queues = phy_dev.getQueueFamilyProperties();
    auto indexes = views::iota(static_cast<uint32_t>(0), queues.size());

    auto index = ranges::find_if(indexes, [&](auto index) {
        return static_cast<bool>(phy_dev.getSurfaceSupportKHR(index, surface));
    });

    return *index;
}

// gets the first queue with the requested family index flags
uint32_t getQueueFamilyIndex(vk::raii::PhysicalDevice& dev, vk::QueueFlagBits flags) {
    auto queues = dev.getQueueFamilyProperties();
    auto indexes = views::iota(static_cast<uint32_t>(0), queues.size());

    auto index_it = ranges::find_if(indexes, [&](auto index) {
        return static_cast<bool>(queues.at(index).queueFlags & flags);
    });

    return *index_it;
}

// gets the provided surface's properties
SurfaceProperties getSurfaceProperties(vk::raii::PhysicalDevice& dev, vk::raii::SurfaceKHR& surface) {
    return SurfaceProperties {
        .capabilities = dev.getSurfaceCapabilitiesKHR(surface),
        .formats = dev.getSurfaceFormatsKHR(surface),
        .present_modes = dev.getSurfacePresentModesKHR(surface),
    };
}

}


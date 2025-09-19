
#include <algorithm>
#include <vector>
#include <ranges>
#include <vulkan/vulkan_raii.hpp>

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

// from a phy_dev and a surface find the 'presentation' queue family
// prioritize being the graphic bit first

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

// get transition bit for vertex buffers

}


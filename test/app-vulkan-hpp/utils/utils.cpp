
#include <algorithm>
#include <vector>
#include <ranges>

#include "utils.hpp"

using std::vector;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

namespace vu {

// from a phy_dev and a surface find the 'presentation' queue family
// prioritize being the graphic bit first

// get graphic bit from a device
uint32_t GetGraphicsQueueFamilyIndex(vk::raii::PhysicalDevice& phy_dev) {
    uint32_t res = 0;
    vector<vk::QueueFamilyProperties> properties = phy_dev.getQueueFamilyProperties();

    for (auto it = properties.begin(); it != properties.end(); ++it) {
        if (it->queueFlags & vk::QueueFlagBits::eGraphics) {
            res = static_cast<uint32_t>(std::distance(properties.begin(), it));
        }
    }

    return res;
}

// returns all the available queue families with the requested indexes
vector<uint32_t> GetGraphicsQueueFamilyIndexes(vk::raii::PhysicalDevice& phy_dev) {
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

// get transition bit for vertex buffers

}


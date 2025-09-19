#pragma once

#include <iostream>
#include <ranges>
#include <algorithm>

#include "ostream_formatters.hpp"

namespace ranges = std::ranges;

/*
 * program utils
 */
namespace ut {

void prettyPrint(const std::string& prompt);

template <ranges::range T>
void print_container(T& cont) {
    std::cout
        << std::format("Printing container with {} elments: ", ranges::size(cont))
        << std::endl
        << "{" << std::endl;
    ranges::for_each(cont, [](auto& e) { std::cout << "    " << e << std::endl; });

    std::cout
        << "}" << std::endl;
}

}

// tuple that represents a physical device and a queue index
using QueuePhyDeviceTup_t = std::tuple<vk::raii::PhysicalDevice, uint32_t>;

namespace vu {

vk::raii::SurfaceKHR createWindowSurface(vk::raii::Instance& instance);
QueuePhyDeviceTup_t getSuitableDevice(vk::raii::Instance& instance, vk::raii::SurfaceKHR& surface);
uint32_t getGraphicsQueueFamilyIndex(vk::raii::PhysicalDevice& phy_dev);
vector<uint32_t> getGraphicsQueueFamilyIndexes(vk::raii::PhysicalDevice& phy_dev);
vector<uint32_t> getPresentationFamilyIndexes(vk::raii::PhysicalDevice& phy_dev, vk::raii::SurfaceKHR& surface);
uint32_t getPresentationFamilyIndex(vk::raii::PhysicalDevice& phy_dev, vk::raii::SurfaceKHR& surface);

}

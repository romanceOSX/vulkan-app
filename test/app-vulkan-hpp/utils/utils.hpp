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

template <ranges::range T>
void print_container(T& cont) {
    ranges::for_each(cont, [](auto& e) { std::cout << "**" << e << std::endl; });
}

}


namespace vu {

uint32_t GetGraphicsQueueFamilyIndex(vk::raii::PhysicalDevice& phy_dev);
vector<uint32_t> GetGraphicsQueueFamilyIndexes(vk::raii::PhysicalDevice& phy_dev);

}

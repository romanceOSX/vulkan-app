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


namespace vu {

uint32_t getGraphicsQueueFamilyIndex(vk::raii::PhysicalDevice& phy_dev);
vector<uint32_t> getGraphicsQueueFamilyIndexes(vk::raii::PhysicalDevice& phy_dev);

}

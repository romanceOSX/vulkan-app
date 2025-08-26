#pragma once

#include <iostream>
#include <ranges>
#include <algorithm>

namespace ranges = std::ranges;

/*
 * program utils
 */
template <ranges::range T>
void utils_print_container(T& cont) {
    ranges::for_each(cont, [](auto& e) { std::cout << "**" << e << std::endl; });
}


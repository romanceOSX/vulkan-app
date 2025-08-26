#pragma once

#include <iostream>
#include <sstream>
#include <ranges>
#include <vector>

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"

namespace ranges = std::ranges;
using std::vector;

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


#pragma once

#include <iostream>
#include <ranges>
#include <algorithm>
#include <vulkan/vulkan.hpp>

#include "ostream_formatters.hpp"

namespace ranges = std::ranges;

//
// program utils
// 
namespace ut {

void prettyPrint(const std::string& prompt);

template <ranges::range T>
void printContainer(T& cont) {
    std::cout
        << std::format("Printing container with {} elments: ", ranges::size(cont))
        << std::endl
        << "{" << std::endl;
    ranges::for_each(cont, [](auto& e) { std::cout << "    " << e << std::endl; });

    std::cout
        << "}" << std::endl;
}

// ptints OK check to stream
void printCheck(std::ostream& ostream);
std::string readFile(const std::string& path);

}

// Tuple that represents a physical device and a queue indexchooseSwapSurfaceFormat
using QueuePhyDeviceTup_t = std::tuple<vk::raii::PhysicalDevice, uint32_t>;

// Surface query properties class wrapper that contains:
//  - Surface Capabilities
//  - Surface Fromat Support
//  - Surface Presentation Mode Support
//  TODO: move this inside vu namespace
//  NOTE: go over what exactly each component specifies
struct SurfaceProperties {
    vk::SurfaceCapabilitiesKHR capabilities;
    vector<vk::SurfaceFormatKHR> formats;
    vector<vk::PresentModeKHR> present_modes;
};

//
// vulkan utitites
//
namespace vu {

vk::raii::SurfaceKHR createWindowSurface(vk::raii::Instance& instance);
QueuePhyDeviceTup_t getSuitableDevice(vk::raii::Instance& instance, vk::raii::SurfaceKHR& surface);
uint32_t getGraphicsQueueFamilyIndex(vk::raii::PhysicalDevice& phy_dev);
vector<uint32_t> getGraphicsQueueFamilyIndexes(vk::raii::PhysicalDevice& phy_dev);
vector<uint32_t> getPresentationFamilyIndexes(vk::raii::PhysicalDevice& phy_dev, vk::raii::SurfaceKHR& surface);
uint32_t getPresentationFamilyIndex(vk::raii::PhysicalDevice& phy_dev, vk::raii::SurfaceKHR& surface);
uint32_t getQueueFamilyIndex(vk::raii::PhysicalDevice& dev, vk::QueueFlagBits flags);
SurfaceProperties getSurfaceProperties(vk::raii::PhysicalDevice& dev, vk::raii::SurfaceKHR& surface);
vk::SurfaceFormatKHR chooseSurfaceFormat(const vector<vk::SurfaceFormatKHR>& formats);      // TODO: implement this function
void glslToSpirV(vk::ShaderStageFlagBits const & shader_type, std::string const & shader_glsl, vector<unsigned int>& shader_spirv);
vk::raii::ShaderModule createShaderModule(vk::raii::Device& dev, vk::ShaderStageFlagBits const & shader_type, std::string& shader_text);

}


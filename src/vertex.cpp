#include <cstddef>
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>

#include "vertex.hpp"

const std::vector<Vertex> vertices = {
};

/* describes which rate to load data from memory througout the vertices */
VkVertexInputBindingDescription VertexInput::get_binding_description() {
}

/* position and color vertex attributes */
std::array<VkVertexInputAttributeDescription, 2> VertexInput::get_attribute_descriptions() {
}


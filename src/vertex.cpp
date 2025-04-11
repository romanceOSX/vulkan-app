#include <cstddef>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>

#include "app_settings.hpp"

#include "vertex.hpp"
#include "device.hpp"

const std::vector<Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
};

/* describes which rate to load data from memory througout the vertices */
VkVertexInputBindingDescription VertexInput::get_binding_description() {
    VkVertexInputBindingDescription binding_description{};
    binding_description.binding = 0;
    binding_description.stride = sizeof(Vertex);
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return binding_description;
}

/* position and color vertex attributes */
std::array<VkVertexInputAttributeDescription, 2> VertexInput::get_attribute_descriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions{};
    /* vertex data */
    attribute_descriptions[0].binding = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_descriptions[0].offset = offsetof(Vertex, pos);

    /* fragment data */
    attribute_descriptions[1].binding = 0;
    attribute_descriptions[1].location = 1;
    attribute_descriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_descriptions[1].offset = offsetof(Vertex, color);

    return attribute_descriptions;
}

VertexBuffer::VertexBuffer(Device& dev): m_device{dev} {
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.pNext = nullptr;
    buffer_info.flags = 0;

    buffer_info.size = sizeof(vertices[0]) * vertices.size();

    buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_device, &buffer_info, nullptr, &m_vk_buffer)
            != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vertex buffer 😵");
    }
    APP_PRETTY_PRINT_CREATE("created vertex buffer");
}

VertexBuffer::~VertexBuffer() {
    vkDestroyBuffer(m_device, m_vk_buffer, nullptr);
    APP_PRETTY_PRINT_DESTROY("destroyed vertex buffer");
}


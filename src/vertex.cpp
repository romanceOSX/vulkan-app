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

    /* query memory requirements */
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(m_device, m_vk_buffer, &mem_requirements);

    /* allocate memory */
    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = _find_memory_type(mem_requirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_CACHED_BIT
            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    if (vkAllocateMemory(m_device, &alloc_info, nullptr, &m_vk_device_memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory");
    }
    APP_PRETTY_PRINT_CREATE("allocated vertex buffer memory");

    /* associate allocated memory with the buffer */
    vkBindBufferMemory(m_device, m_vk_buffer, m_vk_device_memory, 0);
}

/* query memory properties from device */
// TODO: These properties can be queried before creating logical device, move it to Physical Dev instead?
uint32_t VertexBuffer::_find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(m_device.get_vk_physical_dev(), &mem_properties);

    /* different memory types exist within the queried memory heaps */
    /* (memoryTypes, memoryHeaps) */

    for (uint32_t i = 0; i< mem_properties.memoryTypeCount; i++) {
        if (
                (type_filter & (1 << i))
                && ((mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
           )
        {
            return i;
        }
    }
    throw std::runtime_error("couldn't find suitable memory type 😵");
}

VertexBuffer::~VertexBuffer() {
    vkDestroyBuffer(m_device, m_vk_buffer, nullptr);
    APP_PRETTY_PRINT_DESTROY("destroyed vertex buffer");
}


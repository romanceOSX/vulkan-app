#include <cstddef>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>

#include "app_settings.hpp"

#include "physical_device.hpp"
#include "device.hpp"
#include "vertex.hpp"

/*
 * Components:
 *  - shader
 *  - vertex input attribute (per input variable)
 *  - vertex input binding
 *  - buffers
 *
 * Both the attribute and binding provide information on how to interpret the passed data
 *  - Input attribute's describes each of the attributes
 *  - Input binding describes the whole structure passed to it
 *
 * The association between the attribute and bindings happens at pipeline creation
 * The association between the binding and the buffer happens trhough vkCmdBindVertexBuffers
 */

/*
 * This whole module is in charge of describing vulkan how to pass in data to the shader
 * this includes the following:
 *  - the vertex data itself
 *  - any attributes we expect at the vertex shader
 *
 * The overall workflow is the following:
 *  - we tell vulkan how to pass the data from the vertex buffer to the shader
 *    through vertex 'binding descriptions' (size information)
 *  - if there is any vertex attributes we pass in into the shader, we need to
 *    tell vulkan how to do so accordingly through 'attribute descriptions'
 *  - 
 */

/* hello triangle */
const std::vector<Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
};

/* 
 * A vertex binding describes which rate to load data 
 * from memory througout the vertices
 */
VkVertexInputBindingDescription VertexInput::get_binding_description() {
    VkVertexInputBindingDescription binding_description{};
    binding_description.binding = 0;
    binding_description.stride = sizeof(Vertex);
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return binding_description;
}

/* position and color vertex attributes */
/* 
 * Tells vulkan how to pass the data format to the vertex shader
 * --> https://docs.vulkan.org/spec/latest/chapters/fxvertex.html
 */
std::array<VkVertexInputAttributeDescription, 2> VertexInput::get_attribute_descriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions{};
    /* position data */
    attribute_descriptions[0].binding = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_descriptions[0].offset = offsetof(Vertex, pos);

    /* colour data */
    attribute_descriptions[1].binding = 0;
    attribute_descriptions[1].location = 1;
    attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[1].offset = offsetof(Vertex, color);

    return attribute_descriptions;
}

VertexBuffer::VertexBuffer(Device& dev): m_device{dev} {
    auto phy_dev = m_device.get_physical_device();
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.pNext = nullptr;
    buffer_info.flags = 0;

    buffer_info.size = sizeof(vertices[0]) * vertices.size();

    /* This specifies the usage of the memory being allocated */
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
    alloc_info.memoryTypeIndex = phy_dev.find_memory_properties(
        mem_requirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    ).value();

    if (vkAllocateMemory(m_device, &alloc_info, nullptr, &m_vk_device_memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory");
    }
    APP_PRETTY_PRINT_CREATE("allocated vertex buffer memory");

    /* associate allocated memory with the buffer */
    vkBindBufferMemory(m_device, m_vk_buffer, m_vk_device_memory, 0);

    /* copy vertex data into the memory */
    void *data;
    vkMapMemory(m_device, m_vk_device_memory, 0, buffer_info.size, 0, &data);
    memcpy(data, vertices.data(), (size_t) buffer_info.size);
    vkUnmapMemory(m_device, m_vk_device_memory);
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
    vkFreeMemory(m_device, m_vk_device_memory, nullptr);
    APP_PRETTY_PRINT_DESTROY("de-allocate vertex buffer memory");
}

VkBuffer VertexBuffer::get_vk_buffer() {
    return m_vk_buffer;
}


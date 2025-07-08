#include <cstddef>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>

#include "app_settings.hpp"

#include "physical_device.hpp"
#include "device.hpp"
#include "vertex.hpp"
#include "buffer.hpp"

/*
 * Components:
 *  - shader
 *  - vertex input attribute (per input variable)
 *  - vertex input binding
 *  - buffers
 *  - memory
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
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
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

/*
 * This class just allocates the memory and buffer through the desired requirements
 * At this point of the memory creation we do not need to indicate how to parse/decode/
 * interpret the data, we just need to indicate how much size to allocate
 */
VertexBuffer::VertexBuffer(Device& dev, CommandPool& cmd_pool):
    m_device{dev},
    m_cmd_pool{cmd_pool}
{
    void* data = nullptr;
    VkDeviceSize size = sizeof(vertices[0]) * vertices.size();

    /* create staging buffer */
    Buffer staging_buffer{
        m_device,
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    /* pass in reference to buffer to vertex class */
    m_vk_buffer = staging_buffer.get_vk_buf();

    /* copy vertex memory thorugh mapping */
    staging_buffer.map_memory(&data);
    memcpy(data, vertices.data(), (size_t) size);
    staging_buffer.unmap_memory();

    /* create device-local buffer */
    Buffer vertex_buffer{
        m_device,
        size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    };

    /* copy staging buffer from device-visible buffer to device-local buffer */
    Buffer::copy_buffer(m_cmd_pool, staging_buffer, vertex_buffer, size);
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


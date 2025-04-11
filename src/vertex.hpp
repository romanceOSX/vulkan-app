#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>

class Device;

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

};

/* TODO: create a single class? */
class VertexInput {
    public:
        static VkVertexInputBindingDescription get_binding_description();
        static std::array<VkVertexInputAttributeDescription, 2> get_attribute_descriptions(); 
};

/*
 * Buffers in vulkan are regions of memory that can be read by the device,
 * they can store vertex data
 * Buffers do not automatically allocate memory for themselves, hence this class
 */
class VertexBuffer {
    public:
        VertexBuffer(Device& dev);
        ~VertexBuffer();

    private:
        Device&     m_device;
        VkBuffer    m_vk_buffer;
};


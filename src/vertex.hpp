#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

};

class VertexInput {
    public:
        static VkVertexInputBindingDescription get_binding_description();
        static std::array<VkVertexInputAttributeDescription, 2> get_attribute_descriptions(); 

    private:
        Vertex&      m_vector;
};


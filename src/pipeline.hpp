#include <vector>

#include <vulkan/vulkan_core.h>

class Device;

class Pipeline {
    public:
        Pipeline(Device& dev);
        VkShaderModule create_shader_module(std::vector<char>& bytes);

    private:
        Device&     m_device;
};


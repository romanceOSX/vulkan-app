
#include <algorithm>
#include <vector>
#include <ranges>
#include <string>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

// TODO: define these in either cmake or some global config file
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <glslang/Public/resource_limits_c.h>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "utils.hpp"

// glslang related
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>

using std::vector;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

namespace ut {

void prettyPrint(const std::string& prompt) {
    std::cout << std::format("-- {}!", prompt) << std::endl;
}

void printCheck(std::ostream &ostream) {
    ostream << std::format("✅🆗 So far so good") << std::endl;
}

std::string readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in)
        std::runtime_error("Failed to open file!");

    std::string buffer(fs::file_size(path), '\0');
    in.read(buffer.data(), buffer.size());

    return buffer;
}

}

namespace vu {

// finds the graphics queue family
uint32_t findGraphicsQueueFamilyIndex(vk::raii::PhysicalDevice& phy_dev) {
    auto queue_families = phy_dev.getQueueFamilyProperties();

    auto graphics_queue = ranges::find_if(queue_families, [](vk::QueueFamilyProperties& queue){
        return static_cast<bool>(queue.queueFlags & vk::QueueFlagBits::eGraphics);
    });

    assert(graphics_queue != queue_families.end());

    std::cout << "--Found graphics queue!! " << *graphics_queue << std::endl;

    return static_cast<uint32_t>(std::distance(queue_families.begin(), graphics_queue));
}

// finds the best suitable physical device, and queue_family
QueuePhyDeviceTup_t getSuitableDevice(vk::raii::Instance& instance, vk::raii::SurfaceKHR& surface) {
    for (auto& phy_dev : instance.enumeratePhysicalDevices()) {
        auto queues = phy_dev.getQueueFamilyProperties();
        for (auto it = queues.begin(); it != queues.end(); ++it) {
            uint32_t index = static_cast<uint32_t>(std::distance(queues.begin(), it));
            if (phy_dev.getSurfaceSupportKHR(index, surface))
                assert(it != queues.end());
                return std::make_tuple(phy_dev, index);
        }
    }
}

// TODO: make this clearer
vk::raii::SurfaceKHR createWindowSurface(vk::raii::Instance& instance) {
    uint32_t width = 500;
    uint32_t height = 500;
    
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    VkSurfaceKHR _surface;
    GLFWwindow* window = glfwCreateWindow(width, height, "vulkan hpp", nullptr, nullptr);

    assert(window != nullptr);

    if (VK_SUCCESS != glfwCreateWindowSurface(*instance, window, nullptr, &_surface)) {
        throw std::runtime_error("Failed to create window surface ❌");
    }

    return vk::raii::SurfaceKHR(instance, _surface);
}

// create swapchain
void CreateSwapchain(vk::raii::PhysicalDevice& phy_dev, vk::raii::SurfaceKHR& surface) {
}

// get graphic bit from a device
uint32_t getGraphicsQueueFamilyIndex(vk::raii::PhysicalDevice& phy_dev) {
    uint32_t res = 0;
    vector<vk::QueueFamilyProperties> properties = phy_dev.getQueueFamilyProperties();

    for (auto it = properties.begin(); it != properties.end(); ++it) {
        if (it->queueFlags & vk::QueueFlagBits::eGraphics) {
            res = static_cast<uint32_t>(std::distance(properties.begin(), it));
            break;
        }
    }

    return res;
}

// returns all the available queue families with the requested indexes
vector<uint32_t> getGraphicsQueueFamilyIndexes(vk::raii::PhysicalDevice& phy_dev) {
    vector<vk::QueueFamilyProperties> properties = phy_dev.getQueueFamilyProperties();
    size_t count = 0;

    auto indexes = views::iota(static_cast<size_t>(0), properties.size());
    
    auto res_indexes = indexes
        | views::filter([&](size_t index) {
            return static_cast<bool>((properties.at(index).queueFlags) & (vk::QueueFlagBits::eGraphics));
        })
        | ranges::to<vector<uint32_t>>();

    return res_indexes;
}

// graphic and presentation CAN be different
vector<uint32_t> getPresentationFamilyIndexes(vk::raii::PhysicalDevice& phy_dev, vk::raii::SurfaceKHR& surface) {
    auto queue_families = phy_dev.getQueueFamilyProperties();
    auto indexes = views::iota(static_cast<size_t>(0), queue_families.size());

    auto res_indexes = indexes
        | views::filter([&](size_t index) {     // NOTE: why can't I borrow by ref here
            return static_cast<bool>(phy_dev.getSurfaceSupportKHR(index, surface));
        })
        | ranges::to<std::vector<uint32_t>>();
    return res_indexes;
}

// gets a single present queue
uint32_t getPresentationFamilyIndex(vk::raii::PhysicalDevice& phy_dev, vk::raii::SurfaceKHR& surface) {
    auto queues = phy_dev.getQueueFamilyProperties();
    auto indexes = views::iota(static_cast<uint32_t>(0), queues.size());

    auto index = ranges::find_if(indexes, [&](auto index) {
        return static_cast<bool>(phy_dev.getSurfaceSupportKHR(index, surface));
    });

    return *index;
}

// gets the first queue with the requested family index flags
uint32_t getQueueFamilyIndex(vk::raii::PhysicalDevice& dev, vk::QueueFlagBits flags) {
    auto queues = dev.getQueueFamilyProperties();
    auto indexes = views::iota(static_cast<uint32_t>(0), queues.size());

    auto index_it = ranges::find_if(indexes, [&](auto index) {
        return static_cast<bool>(queues.at(index).queueFlags & flags);
    });

    return *index_it;
}

// gets the provided surface's properties
SurfaceProperties getSurfaceProperties(vk::raii::PhysicalDevice& dev, vk::raii::SurfaceKHR& surface) {
    return SurfaceProperties {
        .capabilities = dev.getSurfaceCapabilitiesKHR(surface),
        .formats = dev.getSurfaceFormatsKHR(surface),
        .present_modes = dev.getSurfacePresentModesKHR(surface),
    };
}

// Given a list of image formats, choose the most appropiate surface format
// default implementation looks for the format eB8G8R8A8Srgb, and colorspace eSrgbNonlinear
vk::SurfaceFormatKHR chooseSurfaceFormat(const vector<vk::SurfaceFormatKHR>& formats) {
    // TODO: check for empty 'formats' list  
    auto format_it = ranges::find_if(
        formats,
        [](const vk::SurfaceFormatKHR format) {
            return (format.format == vk::Format::eB8G8R8A8Unorm)        // NOTE: why not eB8G8R8A8Srgb?
                && (format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear);
        }
    );

    // return the first format if not found the appropiate one
    return format_it != formats.end() ? *format_it : formats.at(0);
}

EShLanguage translateShaderStage(vk::ShaderStageFlagBits const & shader_type) {   
    switch (shader_type)
    {
        case vk::ShaderStageFlagBits::eVertex: return EShLangVertex;
        case vk::ShaderStageFlagBits::eTessellationControl: return EShLangTessControl;
        case vk::ShaderStageFlagBits::eTessellationEvaluation: return EShLangTessEvaluation;
        case vk::ShaderStageFlagBits::eGeometry: return EShLangGeometry;
        case vk::ShaderStageFlagBits::eFragment: return EShLangFragment;
        case vk::ShaderStageFlagBits::eCompute: return EShLangCompute;
        case vk::ShaderStageFlagBits::eRaygenNV: return EShLangRayGenNV;
        case vk::ShaderStageFlagBits::eAnyHitNV: return EShLangAnyHitNV;
        case vk::ShaderStageFlagBits::eClosestHitNV: return EShLangClosestHitNV;
        case vk::ShaderStageFlagBits::eMissNV: return EShLangMissNV;
        case vk::ShaderStageFlagBits::eIntersectionNV: return EShLangIntersectNV;
        case vk::ShaderStageFlagBits::eCallableNV: return EShLangCallableNV;
        case vk::ShaderStageFlagBits::eTaskNV: return EShLangTaskNV;
        case vk::ShaderStageFlagBits::eMeshNV: return EShLangMeshNV;
        default: assert( false && "Unknown shader stage" ); return EShLangVertex;
    }
}

static const TBuiltInResource DefaultTBuiltInResource = {
    /* .MaxLights = */ 32,
    /* .MaxClipPlanes = */ 6,
    /* .MaxTextureUnits = */ 32,
    /* .MaxTextureCoords = */ 32,
    /* .MaxVertexAttribs = */ 64,
    /* .MaxVertexUniformComponents = */ 4096,
    /* .MaxVaryingFloats = */ 64,
    /* .MaxVertexTextureImageUnits = */ 32,
    /* .MaxCombinedTextureImageUnits = */ 80,
    /* .MaxTextureImageUnits = */ 32,
    /* .MaxFragmentUniformComponents = */ 4096,
    /* .MaxDrawBuffers = */ 32,
    /* .MaxVertexUniformVectors = */ 128,
    /* .MaxVaryingVectors = */ 8,
    /* .MaxFragmentUniformVectors = */ 16,
    /* .MaxVertexOutputVectors = */ 16,
    /* .MaxFragmentInputVectors = */ 15,
    /* .MinProgramTexelOffset = */ -8,
    /* .MaxProgramTexelOffset = */ 7,
    /* .MaxClipDistances = */ 8,
    /* .MaxComputeWorkGroupCountX = */ 65535,
    /* .MaxComputeWorkGroupCountY = */ 65535,
    /* .MaxComputeWorkGroupCountZ = */ 65535,
    /* .MaxComputeWorkGroupSizeX = */ 1024,
    /* .MaxComputeWorkGroupSizeY = */ 1024,
    /* .MaxComputeWorkGroupSizeZ = */ 64,
    /* .MaxComputeUniformComponents = */ 1024,
    /* .MaxComputeTextureImageUnits = */ 16,
    /* .MaxComputeImageUniforms = */ 8,
    /* .MaxComputeAtomicCounters = */ 8,
    /* .MaxComputeAtomicCounterBuffers = */ 1,
    /* .MaxVaryingComponents = */ 60,
    /* .MaxVertexOutputComponents = */ 64,
    /* .MaxGeometryInputComponents = */ 64,
    /* .MaxGeometryOutputComponents = */ 128,
    /* .MaxFragmentInputComponents = */ 128,
    /* .MaxImageUnits = */ 8,
    /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
    /* .MaxCombinedShaderOutputResources = */ 8,
    /* .MaxImageSamples = */ 0,
    /* .MaxVertexImageUniforms = */ 0,
    /* .MaxTessControlImageUniforms = */ 0,
    /* .MaxTessEvaluationImageUniforms = */ 0,
    /* .MaxGeometryImageUniforms = */ 0,
    /* .MaxFragmentImageUniforms = */ 8,
    /* .MaxCombinedImageUniforms = */ 8,
    /* .MaxGeometryTextureImageUnits = */ 16,
    /* .MaxGeometryOutputVertices = */ 256,
    /* .MaxGeometryTotalOutputComponents = */ 1024,
    /* .MaxGeometryUniformComponents = */ 1024,
    /* .MaxGeometryVaryingComponents = */ 64,
    /* .MaxTessControlInputComponents = */ 128,
    /* .MaxTessControlOutputComponents = */ 128,
    /* .MaxTessControlTextureImageUnits = */ 16,
    /* .MaxTessControlUniformComponents = */ 1024,
    /* .MaxTessControlTotalOutputComponents = */ 4096,
    /* .MaxTessEvaluationInputComponents = */ 128,
    /* .MaxTessEvaluationOutputComponents = */ 128,
    /* .MaxTessEvaluationTextureImageUnits = */ 16,
    /* .MaxTessEvaluationUniformComponents = */ 1024,
    /* .MaxTessPatchComponents = */ 120,
    /* .MaxPatchVertices = */ 32,
    /* .MaxTessGenLevel = */ 64,
    /* .MaxViewports = */ 16,
    /* .MaxVertexAtomicCounters = */ 0,
    /* .MaxTessControlAtomicCounters = */ 0,
    /* .MaxTessEvaluationAtomicCounters = */ 0,
    /* .MaxGeometryAtomicCounters = */ 0,
    /* .MaxFragmentAtomicCounters = */ 8,
    /* .MaxCombinedAtomicCounters = */ 8,
    /* .MaxAtomicCounterBindings = */ 1,
    /* .MaxVertexAtomicCounterBuffers = */ 0,
    /* .MaxTessControlAtomicCounterBuffers = */ 0,
    /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
    /* .MaxGeometryAtomicCounterBuffers = */ 0,
    /* .MaxFragmentAtomicCounterBuffers = */ 1,
    /* .MaxCombinedAtomicCounterBuffers = */ 1,
    /* .MaxAtomicCounterBufferSize = */ 16384,
    /* .MaxTransformFeedbackBuffers = */ 4,
    /* .MaxTransformFeedbackInterleavedComponents = */ 64,
    /* .MaxCullDistances = */ 8,
    /* .MaxCombinedClipAndCullDistances = */ 8,
    /* .MaxSamples = */ 4,
    /* .maxMeshOutputVerticesNV = */ 256,
    /* .maxMeshOutputPrimitivesNV = */ 512,
    /* .maxMeshWorkGroupSizeX_NV = */ 32,
    /* .maxMeshWorkGroupSizeY_NV = */ 1,
    /* .maxMeshWorkGroupSizeZ_NV = */ 1,
    /* .maxTaskWorkGroupSizeX_NV = */ 32,
    /* .maxTaskWorkGroupSizeY_NV = */ 1,
    /* .maxTaskWorkGroupSizeZ_NV = */ 1,
    /* .maxMeshViewCountNV = */ 4,
    /* .maxMeshOutputVerticesEXT = */ 256,
    /* .maxMeshOutputPrimitivesEXT = */ 256,
    /* .maxMeshWorkGroupSizeX_EXT = */ 128,
    /* .maxMeshWorkGroupSizeY_EXT = */ 128,
    /* .maxMeshWorkGroupSizeZ_EXT = */ 128,
    /* .maxTaskWorkGroupSizeX_EXT = */ 128,
    /* .maxTaskWorkGroupSizeY_EXT = */ 128,
    /* .maxTaskWorkGroupSizeZ_EXT = */ 128,
    /* .maxMeshViewCountEXT = */ 4,
    /* .maxDualSourceDrawBuffersEXT = */ 1,

    /* .limits = */ {
        /* .nonInductiveForLoops = */ 1,
        /* .whileLoops = */ 1,
        /* .doWhileLoops = */ 1,
        /* .generalUniformIndexing = */ 1,
        /* .generalAttributeMatrixVectorIndexing = */ 1,
        /* .generalVaryingIndexing = */ 1,
        /* .generalSamplerIndexing = */ 1,
        /* .generalVariableIndexing = */ 1,
        /* .generalConstantMatrixVectorIndexing = */ 1,
    }};
static TBuiltInResource InitResources()
{
    TBuiltInResource Resources;

    Resources.maxLights                                 = 32;
    Resources.maxClipPlanes                             = 6;
    Resources.maxTextureUnits                           = 32;
    Resources.maxTextureCoords                          = 32;
    Resources.maxVertexAttribs                          = 64;
    Resources.maxVertexUniformComponents                = 4096;
    Resources.maxVaryingFloats                          = 64;
    Resources.maxVertexTextureImageUnits                = 32;
    Resources.maxCombinedTextureImageUnits              = 80;
    Resources.maxTextureImageUnits                      = 32;
    Resources.maxFragmentUniformComponents              = 4096;
    Resources.maxDrawBuffers                            = 32;
    Resources.maxVertexUniformVectors                   = 128;
    Resources.maxVaryingVectors                         = 8;
    Resources.maxFragmentUniformVectors                 = 16;
    Resources.maxVertexOutputVectors                    = 16;
    Resources.maxFragmentInputVectors                   = 15;
    Resources.minProgramTexelOffset                     = -8;
    Resources.maxProgramTexelOffset                     = 7;
    Resources.maxClipDistances                          = 8;
    Resources.maxComputeWorkGroupCountX                 = 65535;
    Resources.maxComputeWorkGroupCountY                 = 65535;
    Resources.maxComputeWorkGroupCountZ                 = 65535;
    Resources.maxComputeWorkGroupSizeX                  = 1024;
    Resources.maxComputeWorkGroupSizeY                  = 1024;
    Resources.maxComputeWorkGroupSizeZ                  = 64;
    Resources.maxComputeUniformComponents               = 1024;
    Resources.maxComputeTextureImageUnits               = 16;
    Resources.maxComputeImageUniforms                   = 8;
    Resources.maxComputeAtomicCounters                  = 8;
    Resources.maxComputeAtomicCounterBuffers            = 1;
    Resources.maxVaryingComponents                      = 60;
    Resources.maxVertexOutputComponents                 = 64;
    Resources.maxGeometryInputComponents                = 64;
    Resources.maxGeometryOutputComponents               = 128;
    Resources.maxFragmentInputComponents                = 128;
    Resources.maxImageUnits                             = 8;
    Resources.maxCombinedImageUnitsAndFragmentOutputs   = 8;
    Resources.maxCombinedShaderOutputResources          = 8;
    Resources.maxImageSamples                           = 0;
    Resources.maxVertexImageUniforms                    = 0;
    Resources.maxTessControlImageUniforms               = 0;
    Resources.maxTessEvaluationImageUniforms            = 0;
    Resources.maxGeometryImageUniforms                  = 0;
    Resources.maxFragmentImageUniforms                  = 8;
    Resources.maxCombinedImageUniforms                  = 8;
    Resources.maxGeometryTextureImageUnits              = 16;
    Resources.maxGeometryOutputVertices                 = 256;
    Resources.maxGeometryTotalOutputComponents          = 1024;
    Resources.maxGeometryUniformComponents              = 1024;
    Resources.maxGeometryVaryingComponents              = 64;
    Resources.maxTessControlInputComponents             = 128;
    Resources.maxTessControlOutputComponents            = 128;
    Resources.maxTessControlTextureImageUnits           = 16;
    Resources.maxTessControlUniformComponents           = 1024;
    Resources.maxTessControlTotalOutputComponents       = 4096;
    Resources.maxTessEvaluationInputComponents          = 128;
    Resources.maxTessEvaluationOutputComponents         = 128;
    Resources.maxTessEvaluationTextureImageUnits        = 16;
    Resources.maxTessEvaluationUniformComponents        = 1024;
    Resources.maxTessPatchComponents                    = 120;
    Resources.maxPatchVertices                          = 32;
    Resources.maxTessGenLevel                           = 64;
    Resources.maxViewports                              = 16;
    Resources.maxVertexAtomicCounters                   = 0;
    Resources.maxTessControlAtomicCounters              = 0;
    Resources.maxTessEvaluationAtomicCounters           = 0;
    Resources.maxGeometryAtomicCounters                 = 0;
    Resources.maxFragmentAtomicCounters                 = 8;
    Resources.maxCombinedAtomicCounters                 = 8;
    Resources.maxAtomicCounterBindings                  = 1;
    Resources.maxVertexAtomicCounterBuffers             = 0;
    Resources.maxTessControlAtomicCounterBuffers        = 0;
    Resources.maxTessEvaluationAtomicCounterBuffers     = 0;
    Resources.maxGeometryAtomicCounterBuffers           = 0;
    Resources.maxFragmentAtomicCounterBuffers           = 1;
    Resources.maxCombinedAtomicCounterBuffers           = 1;
    Resources.maxAtomicCounterBufferSize                = 16384;
    Resources.maxTransformFeedbackBuffers               = 4;
    Resources.maxTransformFeedbackInterleavedComponents = 64;
    Resources.maxCullDistances                          = 8;
    Resources.maxCombinedClipAndCullDistances           = 8;
    Resources.maxSamples                                = 4;
    Resources.maxMeshOutputVerticesNV                   = 256;
    Resources.maxMeshOutputPrimitivesNV                 = 512;
    Resources.maxMeshWorkGroupSizeX_NV                  = 32;
    Resources.maxMeshWorkGroupSizeY_NV                  = 1;
    Resources.maxMeshWorkGroupSizeZ_NV                  = 1;
    Resources.maxTaskWorkGroupSizeX_NV                  = 32;
    Resources.maxTaskWorkGroupSizeY_NV                  = 1;
    Resources.maxTaskWorkGroupSizeZ_NV                  = 1;
    Resources.maxMeshViewCountNV                        = 4;

    Resources.limits.nonInductiveForLoops                 = 1;
    Resources.limits.whileLoops                           = 1;
    Resources.limits.doWhileLoops                         = 1;
    Resources.limits.generalUniformIndexing               = 1;
    Resources.limits.generalAttributeMatrixVectorIndexing = 1;
    Resources.limits.generalVaryingIndexing               = 1;
    Resources.limits.generalSamplerIndexing               = 1;
    Resources.limits.generalVariableIndexing              = 1;
    Resources.limits.generalConstantMatrixVectorIndexing  = 1;

    return Resources;
}

// converst GLSL into SPIR-V
void glslToSpirV(
    vk::ShaderStageFlagBits const & shader_type,
    std::string const & shader_glsl,
    vector<unsigned int>& shader_spirv
) {
    glslang::InitializeProcess();
    EShLanguage stage = translateShaderStage(shader_type);

    const char * shader_strings[1];

    shader_strings[0] = shader_glsl.data();

    // set the input glsl shader
    glslang::TShader shader(stage);
    shader.setStrings(shader_strings, 1);

    // enable SPIR-V and vulkan rules when parsing the shader
    EShMessages messages = (EShMessages)( EShMsgSpvRules | EShMsgVulkanRules );

    const TBuiltInResource* init_resources = &DefaultTBuiltInResource;
    
    if ( !shader.parse( init_resources, 100, false, messages ) )
      {
        puts( shader.getInfoLog() );
        puts( shader.getInfoDebugLog() );
        throw std::runtime_error("Something Failed when parsing the shader");  // something didn't work
      }

    // TODO: enable messaging

    glslang::TProgram program;
    program.addShader(&shader);

    if ( !program.link( messages ) )
    {
        puts( shader.getInfoLog() );
        puts( shader.getInfoDebugLog() );
        fflush( stdout );
        throw std::runtime_error("Something Failed when parsing the shader");  // something didn't work
    }

    glslang::GlslangToSpv(*program.getIntermediate(stage), shader_spirv);

    glslang::FinalizeProcess();
}

vk::raii::ShaderModule createShaderModule(
    vk::raii::Device& dev,
    vk::ShaderStageFlagBits const & shader_type,
    std::string& shader_text
) {
    vector<unsigned int> spirv_shader;
    vu::glslToSpirV(shader_type, shader_text, spirv_shader);

    // TODO: add error handling for GLSL to SPIR-V conversion

    vk::ShaderModuleCreateInfo shader_module_create{
        .flags = vk::ShaderModuleCreateFlags(),
        .codeSize = spirv_shader.size() * sizeof(unsigned int),
        .pCode = reinterpret_cast<const uint32_t*>(spirv_shader.data()),
    };

    vk::ShaderModuleCreateFlags();

    return dev.createShaderModule(shader_module_create);
}

}


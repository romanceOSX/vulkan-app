#! /bin/sh

SCRIPT_CWD=
VULKAN_GLSL_COMPILER="glslc"
VULKAN_SDK="$HOME/VulkanSDK/1.4.309.0"
GLSLC_PATH="$VULKAN_SDK/macOS/bin/$VULKAN_GLSL_COMPILER"

SHADER_NAME="triangle"

$GLSLC_PATH shaders/glsl/triangle/$SHADER_NAME.vert -o shaders/glsl/triangle/$SHADER_NAME.vert.spv
$GLSLC_PATH shaders/glsl/triangle/$SHADER_NAME.frag -o shaders/glsl/triangle/$SHADER_NAME.frag.spv


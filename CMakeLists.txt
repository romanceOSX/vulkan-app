cmake_minimum_required(VERSION 3.29)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS Y)
set(CMAKE_FIND_DEBUG_MODE TRUE)

set(CMAKE_BUILD_TYPE Debug)

if (DEFINED ENV{APP_NAME})
    set(APP_OUT_NAME $ENV{APP_NAME})
else()
    set(APP_OUT_NAME vulkan_app)
endif()

# create project
project(${APP_OUT_NAME})

# Where did I get this?
# https://www.glfw.org
set(GLFW_PATH $ENV{HOME}/git/glfw)
set(APP_SRC_DIR src)

# Vulkan SDK path check
if (NOT DEFINED ENV{VULKAN_SDK} OR "$ENV{VULKAN_SDK}" STREQUAL "")
    # findVulkan expects VULKAN_SDK to be an environment variable
    set(ENV{VULKAN_SDK} "~/VulkanSDK/1.4.309.0/macOS")
    message(WARNING "VULKAN_SDK environment variable not provided, defaulting to $ENV{VULKAN_SDK}")
endif()

# GLM lib
# https://github.com/g-truc/glm
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

# Print current Vulkan SDK info if found
if (Vulkan_FOUND)
    message(STATUS "Vulkan VULKAN_SDK: $ENV{VULKAN_SDK}")
    message(STATUS "Vulkan Library path: ${Vulkan_LIBRARY}")
    message(STATUS "Vulkan Include directories: ${Vulkan_INCLUDE_DIRS}")
endif()

## build shaders custom target
## directory target
#set(shaders_out_dir ${CMAKE_BINARY_DIR}/shaders)
#add_custom_target(create_shader_out_dirs
#    COMMAND ${CMAKE_COMMAND} -E make_directory ${shaders_out_dir}
#)
#
## compile vertex shaders
#set(compile_vert_shader "${shaders_out_dir}/triangle.vert.spv")
#add_custom_command(OUTPUT "${compile_vert_shader}"
#    COMMAND Vulkan::glslc
#            ${CMAKE_SOURCE_DIR}/shaders/glsl/triangle/triangle.vert
#            -o ${compile_vert_shader}
#    DEPENDS create_shader_out_dirs
#)
#add_custom_target(build_shaders DEPENDS ${compile_vert_shader})

# create shader's output directory
set(SHADER_OUT_DIR ${CMAKE_BINARY_DIR}/shaders)
set(SHADER_OUT_DIR_MARKER ${SHADER_OUT_DIR}/.dir_mark)

add_custom_command(
    OUTPUT ${SHADER_OUT_DIR_MARKER}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${SHADER_OUT_DIR_MARKER}
    COMMENT "Creating output spirv shader directory at ${SHADER_OUT_DIR}"
)

# compiles the shader
function(compile_shader SHADER_PATH TARGET_NAME)
    get_filename_component(shader_name ${SHADER_PATH} NAME)
    set(shader_out_path ${SHADER_OUT_DIR}/${shader_name}.spirv)
    add_custom_command(OUTPUT ${shader_out_path} 
        COMMAND Vulkan::glslc
                ${SHADER_PATH}
                -o ${shader_out_path}
        DEPENDS ${SHADER_OUT_DIR_MARKER}
        COMMENT "Compiling shader ${SHADER_PATH} to ${shader_out_path}..."
    )
    add_custom_target(build_shader_${shader_name} DEPENDS ${shader_out_path})
    add_dependencies(${TARGET_NAME} build_shader_${shader_name})
endfunction()


set(APP_SOURCES
    ${APP_SRC_DIR}/main.cpp
    ${APP_SRC_DIR}/app.cpp
    ${APP_SRC_DIR}/host.cpp
    ${APP_SRC_DIR}/instance.cpp
    ${APP_SRC_DIR}/device.cpp
    ${APP_SRC_DIR}/physical_device.cpp
    ${APP_SRC_DIR}/command_pool.cpp
    ${APP_SRC_DIR}/window.cpp
    ${APP_SRC_DIR}/swapchain.cpp
    ${APP_SRC_DIR}/pipeline.cpp
    ${APP_SRC_DIR}/framebuffers.cpp
    ${APP_SRC_DIR}/synchronization.cpp
    ${APP_SRC_DIR}/vertex.cpp
)

add_subdirectory(${GLFW_PATH} glfw)
add_executable(${APP_OUT_NAME} ${APP_SOURCES})
compile_shader(${CMAKE_SOURCE_DIR}/shaders/glsl/triangle/triangle.vert ${APP_OUT_NAME})
compile_shader(${CMAKE_SOURCE_DIR}/shaders/glsl/triangle/triangle.frag ${APP_OUT_NAME})

target_link_libraries(${APP_OUT_NAME} Vulkan::Vulkan) 
target_link_libraries(${APP_OUT_NAME} glfw)
# glm header-only library
target_link_libraries(${APP_OUT_NAME} glm::glm-header-only)


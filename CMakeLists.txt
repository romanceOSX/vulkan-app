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
set(APP_SRC_DIR app)

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

# include custom modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
include(ShaderUtils REQUIRED)

# app sources
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
add_subdirectory(utils)

add_executable(${APP_OUT_NAME} ${APP_SOURCES})
compile_shader(${CMAKE_SOURCE_DIR}/shaders/glsl/triangle/triangle.vert ${APP_OUT_NAME})
compile_shader(${CMAKE_SOURCE_DIR}/shaders/glsl/triangle/triangle.frag ${APP_OUT_NAME})

target_link_libraries(${APP_OUT_NAME} Vulkan::Vulkan) 
target_link_libraries(${APP_OUT_NAME} glfw)
target_link_libraries(${APP_OUT_NAME} utils)
target_link_libraries(${APP_OUT_NAME} glm::glm-header-only)

# tests
add_subdirectory(test)


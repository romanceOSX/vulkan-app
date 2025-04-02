cmake_minimum_required(VERSION 3.29)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS Y)
set(CMAKE_FIND_DEBUG_MODE TRUE)

set(CMAKE_BUILD_TYPE Debug)

if (DEFINED ENV{APP_NAME})
    set(APP_OUT_NAME $ENV{APP_NAME})
else()
    set(APP_OUT_NAME vulkan_app)
endif()

# Where did I get this?
# https://www.glfw.org
set(GLFW_PATH $ENV{HOME}/git/glfw)
set(APP_SRC_DIR src)

# GLM lib
# https://github.com/g-truc/glm
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

# Project setup
project(${APP_OUT_NAME})

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
)

add_subdirectory(${GLFW_PATH} glfw)
add_executable(${APP_OUT_NAME} ${APP_SOURCES})

target_link_libraries(${APP_OUT_NAME} Vulkan::Vulkan) 
target_link_libraries(${APP_OUT_NAME} glfw)
# glm header-only library
target_link_libraries(${APP_OUT_NAME} glm::glm-header-only)


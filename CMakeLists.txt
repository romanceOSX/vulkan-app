cmake_minimum_required(VERSION 3.29)

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
set(GLFW_PATH $ENV{HOME}/git/glfw)
set(APP_SRC_DIR src)

project(${APP_OUT_NAME})

set(APP_SOURCES
    ${APP_SRC_DIR}/main.cpp
    ${APP_SRC_DIR}/host.cpp
    ${APP_SRC_DIR}/instance.cpp
    ${APP_SRC_DIR}/device.cpp
    ${APP_SRC_DIR}/physical_device.cpp
)

add_subdirectory(${GLFW_PATH} glfw)
add_executable(${APP_OUT_NAME} ${APP_SOURCES})

add_library(vulkan_lib UNKNOWN IMPORTED)
set_target_properties(vulkan_lib PROPERTIES
    IMPORTED_LOCATION /usr/local/lib/libvulkan.dylib
)
target_include_directories(vulkan_lib INTERFACE /usr/local/include)

target_link_libraries(${APP_OUT_NAME} vulkan_lib) 
target_link_libraries(${APP_OUT_NAME} glfw)



set(APP_SOURCES
    main.cpp
    utils/utils.cpp
)
add_executable(app-vulkan-hpp ${APP_SOURCES})
target_link_libraries(app-vulkan-hpp Vulkan::Vulkan)
target_link_libraries(app-vulkan-hpp glfw)


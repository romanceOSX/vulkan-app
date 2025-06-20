# general SPIRV/GLSL shader utilities

# Define output shader directory dependency
set(SHADER_OUT_DIR ${CMAKE_BINARY_DIR}/shaders)
set(SHADER_OUT_DIR_MARKER ${SHADER_OUT_DIR}/.dir_mark)

add_custom_command(
    OUTPUT ${SHADER_OUT_DIR_MARKER}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${SHADER_OUT_DIR_MARKER}
    COMMENT "Creating output spirv shader directory at ${SHADER_OUT_DIR}"
)

# Compiles the specified shader by SHADER_PATH 
# and creates a dependency on TARGET_NAME
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




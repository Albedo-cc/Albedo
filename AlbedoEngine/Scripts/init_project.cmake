if(NOT TARGET ${ALBEDO_APP_NAME})
    message(FATAL_ERROR "Please init project after creating app target.")
endif()

# Generate Folders
set(ALBEDO_APP_FOLDERS
    "asset"
    "cache"
    "config")
foreach(FOLDER_NAME ${ALBEDO_APP_FOLDERS})
    add_custom_command(
        TARGET ${ALBEDO_APP_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ALBEDO_APP_BINARY_DIR}/${FOLDER_NAME}"
        COMMENT "Creating folder: ${ALBEDO_APP_BINARY_DIR}/${FOLDER_NAME}"
    )
endforeach()

# Compile Shaders
set(ALBEDO_SHADER_COMPILER ${Vulkan_GLSLC_EXECUTABLE})
file(GLOB_RECURSE ALBEDO_SHADER_SOURCE_FILES
    "${ALBEDO_APP_SOURCE_DIR}/asset/shaders/*.vert"
    "${ALBEDO_APP_SOURCE_DIR}/asset/shaders/*.frag")

foreach(SHADER_SOURCE_FILE ${ALBEDO_SHADER_SOURCE_FILES})   
    get_filename_component(FILE_NAME ${SHADER_SOURCE_FILE} NAME)
    set(SHADER_SPIRV_FILE "${ALBEDO_APP_BINARY_DIR}/asset/shader/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT  ${SHADER_SPIRV_FILE}
        COMMAND ${ALBEDO_SHADER_COMPILER}
        ARGS    ${SHADER_SOURCE_FILE} -o ${SHADER_SPIRV_FILE}
        DEPENDS ${SHADER_SOURCE_FILE}
        COMMENT "Compiling Shader(${FILE_NAME})\n"
    )
    
    list(APPEND ALBEDO_SHADER_SPIRV_FILES ${SHADER_SPIRV_FILE})
endforeach(SHADER_SOURCE_FILE)

add_custom_target(compile_shaders DEPENDS ${ALBEDO_SHADER_SPIRV_FILES})
set_target_properties(compile_shaders PROPERTIES FOLDER "AlbedoEngine/Automation")

# Copy Assets
add_custom_command(
    TARGET ${ALBEDO_APP_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_directory_if_different
        "${ALBEDO_APP_SOURCE_DIR}/asset"
        "${ALBEDO_APP_BINARY_DIR}/asset"
    COMMENT "Copying APP assets"
)

# Copy Configs
add_custom_command(
    TARGET ${ALBEDO_APP_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_directory_if_different
        "${ALBEDO_APP_SOURCE_DIR}/config"
        "${ALBEDO_APP_BINARY_DIR}/config"
    COMMENT "Copying APP assets"
)
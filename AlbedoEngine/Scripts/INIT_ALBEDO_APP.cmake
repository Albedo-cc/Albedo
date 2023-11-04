if(NOT TARGET ${ALBEDO_APP_NAME})
    message(FATAL_ERROR "Please init project after creating app target.")
endif()

# Generate Folders
set(ALBEDO_APP_FOLDERS
    "asset" "asset/font" "asset/icon" "asset/image" "asset/shader"
    "asset/texture" "asset/texture/1D" "asset/texture/2D" "asset/texture/3D"  "asset/texture/Cubemap"
    "cache"
    "config"
    "settings")
foreach(FOLDER_NAME ${ALBEDO_APP_FOLDERS})
    add_custom_command(
        TARGET ${ALBEDO_APP_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ALBEDO_APP_BINARY_DIR}/${FOLDER_NAME}"
        COMMENT "Creating folder: ${ALBEDO_APP_BINARY_DIR}/${FOLDER_NAME}"
    )
endforeach()

# Update Assets
set(UPDATE_FOLDERS "font" "icon" "image")
foreach(UPDATE_FOLDER ${UPDATE_FOLDERS})
    add_custom_command(
        TARGET ${ALBEDO_APP_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_directory_if_different
            "${ALBEDO_APP_SOURCE_DIR}/asset/${UPDATE_FOLDER}"
            "${ALBEDO_APP_BINARY_DIR}/asset/${UPDATE_FOLDER}"
        COMMENT "Updating APP assets\n"
    )
endforeach()

# Update Shaders
set(ALBEDO_SHADER_COMPILER ${Vulkan_GLSLC_EXECUTABLE})
file(GLOB_RECURSE ALBEDO_APP_SHADERS
    "${ALBEDO_APP_SOURCE_DIR}/asset/shader/*.vert"
    "${ALBEDO_APP_SOURCE_DIR}/asset/shader/*.frag")
foreach(SHADER_SOURCE_FILE ${ALBEDO_APP_SHADERS})   
    get_filename_component(FILE_NAME ${SHADER_SOURCE_FILE} NAME)
    set(SHADER_OUTPUT_PATH "${ALBEDO_APP_BINARY_DIR}/asset/shader")
    set(SHADER_SPIRV_FILE  "${SHADER_OUTPUT_PATH}/${FILE_NAME}.spv")
    add_custom_command(
        TARGET ${ALBEDO_APP_NAME}
        POST_BUILD
        COMMAND ${ALBEDO_SHADER_COMPILER}
        ARGS    ${SHADER_SOURCE_FILE} -o ${SHADER_SPIRV_FILE}
        #DEPENDS ${SHADER_SOURCE_FILE}
        COMMENT "Compiling Shader(${FILE_NAME})\n"
    )
endforeach(SHADER_SOURCE_FILE)

# Update Textures
set(TEXTURE_TYPES "1D" "2D" "3D") #  "Cubemap" not supported yet
foreach(TEXTURE_TYPE ${TEXTURE_TYPES})
    file(GLOB_RECURSE ALBEDO_APP_TEXTURES
        "${ALBEDO_APP_SOURCE_DIR}/asset/texture/${TEXTURE_TYPE}/*")
    foreach(TEXTURE_FILE ${ALBEDO_APP_TEXTURES})
        get_filename_component(FILE_NAME ${TEXTURE_FILE} NAME_WE)
        set(TEXTURE_OUT_PATH "${ALBEDO_APP_BINARY_DIR}/asset/texture/${TEXTURE_TYPE}")
        set(KTX_TEXTURE  "${TEXTURE_OUT_PATH}/${FILE_NAME}.ktx")
        add_custom_command(
            TARGET ${ALBEDO_APP_NAME}
            POST_BUILD
            COMMAND $ENV{ALBEDO_ENGINE_TEXTURE_PROCESSOR}
            ARGS    create --format B8G8R8A8_SRGB
                    ${TEXTURE_FILE} ${KTX_TEXTURE}
            COMMENT "Creating KTX Texture(${FILE_NAME})\n"
        )
    endforeach(TEXTURE_FILE)
endforeach(TEXTURE_TYPE ${TEXTURE_TYPES})

# Update Configs
add_custom_command(
    TARGET ${ALBEDO_APP_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_directory_if_different
        "${ALBEDO_APP_SOURCE_DIR}/config"
        "${ALBEDO_APP_BINARY_DIR}/config"
    COMMENT "Updating APP configs\n"
)

# Update Settings
add_custom_command(
    TARGET ${ALBEDO_APP_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_directory_if_different
        "${ALBEDO_APP_SOURCE_DIR}/settings"
        "${ALBEDO_APP_BINARY_DIR}/settings"
    COMMENT "Updating APP settings\n"
)
add_library(ALBEDO_EXTERN_LIBS INTERFACE)
include("vcpkg/scripts/buildsystems/vcpkg.cmake")

# [Manager]
add_custom_target(${ALBEDO_SDK_NAME} ALL) # For adding denpendencies
set_target_properties(${ALBEDO_SDK_NAME} PROPERTIES FOLDER "${PROJECT_NAME}")

if (NOT TARGET Vulkan::Vulkan)
    message(STATUS "\nLoading Vulkan (Vulkan::Vulkan)")
    find_package(Vulkan REQUIRED)
    target_link_libraries(ALBEDO_EXTERN_LIBS INTERFACE Vulkan::Vulkan)
endif()

# GLFW3
if (NOT TARGET glfw)
    message(STATUS "\nLoading GLFW3 (glfw)")
    find_package(glfw3 REQUIRED)
    target_link_libraries(ALBEDO_EXTERN_LIBS INTERFACE glfw)
    add_custom_command(
        TARGET ${ALBEDO_SDK_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:glfw>
        $<TARGET_FILE_DIR:${ALBEDO_APP_NAME}>
    )
endif()

# VMA
if(NOT TARGET GPUOpen::VulkanMemoryAllocator)
    message(STATUS "\nLoading VMA (GPUOpen::VulkanMemoryAllocator)")
    find_package(VulkanMemoryAllocator REQUIRED)
    target_link_libraries(ALBEDO_EXTERN_LIBS INTERFACE GPUOpen::VulkanMemoryAllocator)
endif()

# KTX
if(NOT TARGET KTX::ktx)
    message(STATUS "\nLoading Khronos-Texture (Ktx)")
    find_package(Ktx REQUIRED)
    target_link_libraries(ALBEDO_EXTERN_LIBS INTERFACE KTX::ktx)
    add_custom_command(
        TARGET ${ALBEDO_SDK_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:KTX::ktx>
        $<TARGET_FILE_DIR:${ALBEDO_APP_NAME}>
    )

    add_custom_command(
        TARGET ${ALBEDO_SDK_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:zstd::libzstd_shared>
        $<TARGET_FILE_DIR:${ALBEDO_APP_NAME}>
    )
endif()

# Dear ImGui
if (NOT TARGET imgui::imgui)
    message(STATUS "\nLoading Dear ImGui-Docking (imgui::imgui)")
    find_package(imgui REQUIRED)
    target_link_libraries(ALBEDO_EXTERN_LIBS INTERFACE imgui::imgui)
endif()

# Spdlog
if (NOT TARGET spdlog)
    message(STATUS "\nLoading Spdlog (spdlog::spdlog)")
    find_package(spdlog REQUIRED)
    target_link_libraries(ALBEDO_EXTERN_LIBS INTERFACE spdlog::spdlog)
    add_custom_command(
        TARGET ${ALBEDO_SDK_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:spdlog::spdlog>
        $<TARGET_FILE_DIR:${ALBEDO_APP_NAME}>
    )
    add_custom_command(
        TARGET ${ALBEDO_SDK_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:fmt::fmt>
        $<TARGET_FILE_DIR:${ALBEDO_APP_NAME}>
    )
endif()

# Tiny glTF
if (NOT TARGET tinygltf)
    message(STATUS "\nLoading Tiny glTF (tinygltf)")
    find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
    target_include_directories(ALBEDO_EXTERN_LIBS INTERFACE ${TINYGLTF_INCLUDE_DIRS})
endif()

# Nlohmann JSON
if (NOT TARGET nlohmann_json::nlohmann_json)
    message(STATUS "\nLoading Nlohmann JSON (nlohmann_json::nlohmann_json)")
    find_package(nlohmann_json REQUIRED)
    target_link_libraries(ALBEDO_EXTERN_LIBS INTERFACE nlohmann_json::nlohmann_json)
endif()

# Eigen
if (NOT TARGET Eigen3::Eigen)
    message(STATUS "\nLoading Eigen3 (eigen)")
    find_package(Eigen3 REQUIRED)
    target_link_libraries(ALBEDO_EXTERN_LIBS INTERFACE Eigen3::Eigen)
endif()

# CGAL
if (NOT TARGET CGAL::CGAL)
    message(STATUS "\nLoading CGAL (CGAL::CGAL)")    
    find_package(CGAL REQUIRED)
    target_link_libraries(ALBEDO_EXTERN_LIBS INTERFACE CGAL::CGAL)
endif()
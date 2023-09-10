add_library(ALBEDO_SDK_EXTERNS INTERFACE)

# Vulkan SDK
find_package(Vulkan REQUIRED)
target_include_directories(ALBEDO_SDK_EXTERNS INTERFACE ${Vulkan_INCLUDE_DIR})

# GLFW3
if (NOT TARGET glfw)
    message(STATUS "\nLoading GLFW3 (glfw)")
    option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
    option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
    option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
    option(GLFW_INSTALL "Generate installation target" OFF)
    option(GLFW_VULKAN_STATIC "Assume the Vulkan loader is linked with the application" OFF)
    add_subdirectory(glfw-3.3.8)

    target_link_libraries(ALBEDO_SDK_EXTERNS INTERFACE glfw)
    set_target_properties(glfw PROPERTIES FOLDER "${PROJECT_NAME}/Extern/GLFW")
    set_target_properties(update_mappings PROPERTIES FOLDER "${PROJECT_NAME}/Extern/GLFW")
endif()

# VMA
if(NOT TARGET VulkanMemoryAllocator)
    message(STATUS "\nLoading VMA (VulkanMemoryAllocator)")
    add_subdirectory(VulkanMemoryAllocator-3.0.1)

    target_link_libraries(ALBEDO_SDK_EXTERNS INTERFACE VulkanMemoryAllocator)
    set_target_properties(VulkanMemoryAllocator PROPERTIES FOLDER "${PROJECT_NAME}/Extern/VMA")
endif()

# Dear ImGui
if (NOT TARGET imgui)
    message(STATUS "\nLoading Dear ImGui Docking(imgui)")
    add_subdirectory(imgui-1.89.9-docking)

    target_link_libraries(ALBEDO_SDK_EXTERNS INTERFACE imgui)
    set_target_properties(imgui PROPERTIES FOLDER "${PROJECT_NAME}/Extern/ImGui")
endif()

# Spdlog
if (NOT TARGET spdlog)
    message(STATUS "\nLoading spdlog")
    option(SPDLOG_USE_STD_FORMAT "Use std::format instead of fmt library." OFF)
    add_subdirectory(spdlog)
    
    target_link_libraries(ALBEDO_SDK_EXTERNS INTERFACE spdlog)
    set_target_properties(spdlog PROPERTIES FOLDER "${PROJECT_NAME}/Extern/Spdlog")
endif()

# Stb
if (NOT TARGET stb)
    message(STATUS "\nLoading stb")
    add_subdirectory(stb)

    target_link_libraries(ALBEDO_SDK_EXTERNS INTERFACE stb)
    set_target_properties(stb PROPERTIES FOLDER "${PROJECT_NAME}/Extern/Stb")
endif()

# Eigen
if (NOT TARGET eigen)
    message(STATUS "\nLoading Eigen3 (eigen)")
    add_subdirectory(Eigen-3.4.0)

    target_link_libraries(ALBEDO_SDK_EXTERNS INTERFACE eigen)
    set_target_properties(eigen PROPERTIES FOLDER "${PROJECT_NAME}/Extern/Eigen")
endif()

# Tiny glTF
if (NOT TARGET tinygltf)
    message(STATUS "\nLoading Tiny glTF (tinygltf)")
    option(TINYGLTF_BUILD_LOADER_EXAMPLE "Build loader_example(load glTF and dump infos)" OFF)
    set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
    set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
    add_subdirectory(tinygltf-2.8.5)

    target_link_libraries(ALBEDO_SDK_EXTERNS INTERFACE tinygltf)
    target_sources(tinygltf PRIVATE "tinygltf-2.8.5/tiny_gltf.h" "tinygltf-2.8.5/tiny_gltf.cc")
    set_target_properties(tinygltf PROPERTIES FOLDER "${PROJECT_NAME}/Extern/TinyglTF")
endif()
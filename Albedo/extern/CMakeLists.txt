# Load Libraries
add_library(AlbedoExternalLibraries INTERFACE)

# Albedo RHI
if (NOT TARGET AlbedoRHI)
    message(STATUS "\nLoading Albedo RHI (Albedo::RHI)")
    add_subdirectory(AlbedoRHI)
    target_link_libraries(AlbedoExternalLibraries INTERFACE Albedo::RHI)
endif()

# Albedo Log
if (NOT TARGET AlbedoLog)
    message(STATUS "\nLoading Albedo Log (Albedo::log)")
    add_subdirectory(AlbedoLog)
    target_link_libraries(AlbedoExternalLibraries INTERFACE Albedo::log)
endif()

# Albedo Net
if (NOT TARGET AlbedoNet)
    message(STATUS "\nLoading Albedo Net (Albedo::net)")
    add_subdirectory(AlbedoNet)
    target_link_libraries(AlbedoExternalLibraries INTERFACE Albedo::net)
endif()

# Albedo Utils
if (NOT TARGET AlbedoUtils)
    message(STATUS "\nLoading Albedo Utils (Albedo::utils)")
    add_subdirectory(AlbedoUtils)
    target_link_libraries(AlbedoExternalLibraries INTERFACE Albedo::utils)
endif()

# Eigen
if (NOT TARGET eigen)
    message(STATUS "\nLoading Eigen3 (eigen)")
    add_subdirectory(Eigen-3.4.0)
    target_link_libraries(AlbedoExternalLibraries INTERFACE eigen)
endif()

# stb_image
if (NOT TARGET stb_image)
    message(STATUS "\nLoading stb_image (stb_image)")
    add_subdirectory(stb_image-2023.1.29)
    target_link_libraries(AlbedoExternalLibraries INTERFACE stb_image)
endif()

# Tiny glTF
if (NOT TARGET tinygltf)
    message(STATUS "\nLoading Tiny glTF (tinygltf)")
    option(TINYGLTF_BUILD_LOADER_EXAMPLE "Build loader_example(load glTF and dump infos)" OFF)
    set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
    set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
    add_subdirectory(tinygltf-2.8.5)
    target_link_libraries(AlbedoExternalLibraries INTERFACE tinygltf)
endif()

# Dear ImGui
if (NOT TARGET imgui)
    message(STATUS "\nLoading Dear ImGui (imgui)")
    add_subdirectory(imgui-1.89.5-docking)
    target_link_libraries(imgui AlbedoRHI)

    target_link_libraries(AlbedoExternalLibraries INTERFACE imgui)
endif()
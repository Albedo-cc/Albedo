cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

project("Albedo"
    LANGUAGES CXX
    VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Explicitly require the standard
set(CMAKE_CXX_EXTENSIONS OFF) # Vendor-specific Extensions

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/_script")

include(check_in_source_build)

add_subdirectory(Albedo)

# Put targets into Folders
include(target_put_into_folder)
target_put_into_folder(Albedo "Albedo")
target_put_into_folder(AlbedoShader "Albedo")

target_put_into_folder(imgui "Albedo/Extern/Dear_ImGUI")
target_put_into_folder(imcurio "Albedo/Extern/Dear_ImCurio")
target_put_into_folder(protobuf "Albedo/Extern/Protocol_Buffer")

target_put_into_folder(AlbedoNet "Albedo/Extern/AlbedoNet")
    target_put_into_folder(asio "Albedo/Extern/AlbedoNet/Library/ASIO")
    target_put_into_folder(cryptopp "Albedo/Extern/AlbedoNet/Library/Crypto++")

target_put_into_folder(AlbedoRHI "Albedo/Extern/AlbedoRHI")
    target_put_into_folder(glfw "Albedo/Extern/AlbedoRHI/Library/GLFW3")
        target_put_into_folder(update_mappings "Albedo/Extern/AlbedoRHI/Library/GLFW3") 
    target_put_into_folder(VulkanMemoryAllocator "Albedo/Extern/AlbedoRHI/Library/VMA")
    target_put_into_folder(spirv-reflect-static "Albedo/Extern/AlbedoRHI/Library/SPIRV-Reflect")

target_put_into_folder(AlbedoLog "Albedo/Extern/AlbedoLog")
    target_put_into_folder(spdlog "Albedo/Extern/AlbedoLog/Library/spdlog")

target_put_into_folder(AlbedoUtils "Albedo/Extern/AlbedoUtils")

# StartUp Project in Visual Studio
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
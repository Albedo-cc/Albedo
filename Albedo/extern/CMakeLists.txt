# Load Libraries
add_library(AlbedoExternalLibraries INTERFACE)
# Albedo Log
if (NOT TARGET AlbedoLog)
    message(STATUS "\nLoading Albedo Log (Albedo::log)")
    add_subdirectory(AlbedoLog)
    target_link_libraries(AlbedoExternalLibraries INTERFACE Albedo::log)
endif()

# Albedo Utils
if (NOT TARGET AlbedoUtils)
    message(STATUS "\nLoading Albedo Utils (Albedo::utils)")
    add_subdirectory(AlbedoUtils)
    target_link_libraries(AlbedoExternalLibraries INTERFACE Albedo::utils)
endif()

# GLFW3
if (NOT TARGET glfw)
    message(STATUS "\nLoading GLFW3 (glfw)")
    option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
    option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
    option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
    option(GLFW_INSTALL "Generate installation target" OFF)
    option(GLFW_VULKAN_STATIC "Assume the Vulkan loader is linked with the application" OFF)
    add_subdirectory(glfw-3.3.8)
    target_link_libraries(AlbedoExternalLibraries INTERFACE glfw)
endif()

# VMA & Vulkan
if(NOT TARGET VulkanMemoryAllocator)
    message(STATUS "\nLoading VMA (VulkanMemoryAllocator)")
    add_subdirectory(VulkanMemoryAllocator-3.0.1)
    target_link_libraries(AlbedoExternalLibraries INTERFACE VulkanMemoryAllocator)
endif()
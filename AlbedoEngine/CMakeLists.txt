cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

set(ALBEDO_ENGINE_NAME "AlbedoEngine")
project(${ALBEDO_ENGINE_NAME}
    LANGUAGES       CXX
    HOMEPAGE_URL    "albedo.cc"
)

# [Configuration]
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Explicitly require the standard
set(CMAKE_CXX_EXTENSIONS OFF) # Vendor-specific Extensions

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/Scripts")
include(check_in_source_build)

# [Options]
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# [Macros]

# [Project]
add_library(${ALBEDO_ENGINE_NAME} INTERFACE)
add_subdirectory(Runtime)
add_subdirectory(Editor)

target_link_libraries(
    ${ALBEDO_ENGINE_NAME}
    INTERFACE
    AlbedoRuntime
    AlbedoEditor
)

file(GLOB ALBEDO_MODULES "Modules/*")
source_group("Modules" FILES ${ALBEDO_MODULES})
target_include_directories(
    ${ALBEDO_ENGINE_NAME}
    INTERFACE "${PROJECT_SOURCE_DIR}/Modules"
)

# [Manager]
set(ALBEDO_SDK_NAME "AlbedoSDK")
add_custom_target(${ALBEDO_SDK_NAME}) # For adding denpendencies
target_sources(${ALBEDO_SDK_NAME} PRIVATE ${ALBEDO_MODULES})
set_target_properties(${ALBEDO_SDK_NAME} PROPERTIES FOLDER "${PROJECT_NAME}")
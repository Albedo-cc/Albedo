cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

set(ALBEDO_ENGINE_NAME  "AlbedoEngine"  )
set(ALBEDO_RUNTIME_NAME "AlbedoRuntime" )
set(ALBEDO_EDITOR_NAME  "AlbedoEditor"  )
set(ALBEDO_SDK_NAME     "AlbedoSDK"     )
project(${ALBEDO_ENGINE_NAME}
    LANGUAGES       CXX
    HOMEPAGE_URL    "albedo.cc"
)

# [Environment]
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Explicitly require the standard
set(CMAKE_CXX_EXTENSIONS OFF) # Vendor-specific Extensions
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/Scripts")
include(CHECK_IN_SOURCE_BUILD)

set(ENV{ALBEDO_ENGINE_TEXTURE_PROCESSOR} "${PROJECT_SOURCE_DIR}/Binary/Windows/x64/ktx.exe")

# [Options]
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# [Macros]

# [Project]
add_library(${ALBEDO_ENGINE_NAME} INTERFACE)
add_subdirectory(Extern)
add_subdirectory(Runtime)
add_subdirectory(Editor)

target_link_libraries(
    ${ALBEDO_ENGINE_NAME}
    INTERFACE
    ${ALBEDO_RUNTIME_NAME}
    ${ALBEDO_EDITOR_NAME}
)
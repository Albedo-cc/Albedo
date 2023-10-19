cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

project("AlbedoSDK"
    LANGUAGES CXX
    HOMEPAGE_URL "albedo.cc")

# [Configuration]
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Explicitly require the standard
set(CMAKE_CXX_EXTENSIONS OFF) # Vendor-specific Extensions

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/script")
include(check_in_source_build)

# [Options]
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# [Macros]


# [Project]
file(GLOB_RECURSE ALBEDO_SDK_HEADER "source/*.h")
file(GLOB_RECURSE ALBEDO_SDK_SOURCE "source/*.cc")
source_group(TREE "${PROJECT_SOURCE_DIR}/source" FILES ${ALBEDO_SDK_HEADER} ${ALBEDO_SDK_SOURCE})
add_library(${PROJECT_NAME} STATIC ${ALBEDO_SDK_HEADER} ${ALBEDO_SDK_SOURCE})
add_subdirectory(extern)
target_link_libraries(${PROJECT_NAME} PUBLIC ALBEDO_SDK_EXTERNS)
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/source")
target_include_directories(${PROJECT_NAME} INTERFACE "${PROJECT_SOURCE_DIR}/modules")
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${PROJECT_NAME}")
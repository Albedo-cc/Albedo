cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

set(ALBEDO_NAME "AlbedoSDK")
project(${ALBEDO_NAME}
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
set(ALBEDO_PIPELINE_CACHE_PATH "$<TARGET_FILE_DIR:${ALBEDO_APP_NAME}>")
set(ALBEDO_PIPELINE_CACHE_NAME "pipeline.cache")
add_compile_definitions(ALBEDO_PIPELINE_CACHE_FILE="${ALBEDO_PIPELINE_CACHE_PATH}/${ALBEDO_PIPELINE_CACHE_NAME}")

# [Project]
file(GLOB_RECURSE ALBEDO_HEADER "SDK/*.h")
file(GLOB_RECURSE ALBEDO_SOURCE "SDK/*.cc")
source_group(TREE "${PROJECT_SOURCE_DIR}/SDK" FILES ${ALBEDO_HEADER} ${ALBEDO_SOURCE})
add_library(${ALBEDO_NAME} STATIC ${ALBEDO_HEADER} ${ALBEDO_SOURCE})
add_subdirectory(extern)
target_link_libraries(${ALBEDO_NAME} PUBLIC ALBEDO_EXTERNS)
target_include_directories(${ALBEDO_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/SDK")
set_target_properties(${ALBEDO_NAME} PROPERTIES FOLDER "${ALBEDO_NAME}")
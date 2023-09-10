cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

set(ALBEDO_APP_NAME "AlbedoAPP")
project(${ALBEDO_APP_NAME}
    LANGUAGES CXX
    VERSION "0.0.0"
    HOMEPAGE_URL "")

# [Configuration]
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Explicitly require the standard
set(CMAKE_CXX_EXTENSIONS OFF) # Vendor-specific Extensions

# [Options]

# [Macros]
add_compile_definitions(ALBEDO_APP_NAME="${ALBEDO_APP_NAME}")
add_compile_definitions(ALBEDO_APP_VERSION="${PROJECT_VERSION}")
add_compile_definitions(ALBEDO_APP_ASSET_PATH="$<TARGET_FILE_DIR:${PROJECT_NAME}>/asset")

# [Project]
add_subdirectory(AlbedoSDK)
file(GLOB_RECURSE ALBEDO_APP_HEADER "APP/*.h")
file(GLOB_RECURSE ALBEDO_APP_SOURCE "APP/*.cc")
file(GLOB_RECURSE ALBEDO_APP_SETTING "APP/setting/*")
source_group(TREE "${PROJECT_SOURCE_DIR}/APP" FILES ${ALBEDO_APP_HEADER} ${ALBEDO_APP_SOURCE} ${ALBEDO_APP_SETTING})
add_executable( ${ALBEDO_APP_NAME} 
                ${ALBEDO_APP_HEADER} 
                ${ALBEDO_APP_SOURCE}
                ${ALBEDO_APP_SETTING})
target_link_libraries(${ALBEDO_APP_NAME} PRIVATE AlbedoSDK)
target_sources(${ALBEDO_APP_NAME} PRIVATE README.md TODO.md)

# [Property]
# Visual Studio
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${ALBEDO_APP_NAME})
set_property(TARGET ${ALBEDO_APP_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
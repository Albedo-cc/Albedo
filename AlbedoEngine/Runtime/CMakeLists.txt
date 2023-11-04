# AlbedoRuntime
file(GLOB_RECURSE ALBEDO_RUNTIME_HEADER "Albedo/*.h")
file(GLOB_RECURSE ALBEDO_RUNTIME_SOURCE "Albedo/*.cc")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Albedo"
             FILES ${ALBEDO_RUNTIME_HEADER} ${ALBEDO_RUNTIME_SOURCE})

add_library(
    ${ALBEDO_RUNTIME_NAME}
    STATIC
    ${ALBEDO_RUNTIME_HEADER}
    ${ALBEDO_RUNTIME_SOURCE}
)

target_link_libraries(
    ${ALBEDO_RUNTIME_NAME} PUBLIC
    ALBEDO_EXTERN_LIBS)

target_include_directories(
    ${ALBEDO_RUNTIME_NAME} PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(
    ${ALBEDO_RUNTIME_NAME} 
    PROPERTIES FOLDER "${PROJECT_NAME}")

# Modules
file(GLOB ALBEDO_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/Modules/*")
source_group("Modules" FILES ${ALBEDO_MODULES})
target_include_directories(
    ${ALBEDO_RUNTIME_NAME}
    INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/Modules"
)
target_sources(${ALBEDO_SDK_NAME} PRIVATE ${ALBEDO_MODULES})
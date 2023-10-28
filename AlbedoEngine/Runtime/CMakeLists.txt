# AlbedoRuntime
file(GLOB_RECURSE ALBEDO_RUNTIME_HEADER "source/*.h")
file(GLOB_RECURSE ALBEDO_RUNTIME_SOURCE "source/*.cc")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/source"
             FILES ${ALBEDO_RUNTIME_HEADER} ${ALBEDO_RUNTIME_SOURCE})

add_library(
    AlbedoRuntime STATIC
    ${ALBEDO_RUNTIME_HEADER} 
    ${ALBEDO_RUNTIME_SOURCE}
)
add_subdirectory(extern)
target_link_libraries(
    AlbedoRuntime PUBLIC 
    ALBEDO_RUNTIME_EXTERNS)
target_include_directories(
    AlbedoRuntime PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/source")
set_target_properties(
    AlbedoRuntime 
    PROPERTIES FOLDER "${PROJECT_NAME}/Runtime")
# AlbedoEditor
file(GLOB_RECURSE ALBEDO_EDITOR_HEADER "source/*.h")
file(GLOB_RECURSE ALBEDO_EDITOR_SOURCE "source/*.cc")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/source"
             FILES ${ALBEDO_EDITOR_HEADER} ${ALBEDO_EDITOR_SOURCE})

add_library(
    AlbedoEditor STATIC
    ${ALBEDO_EDITOR_HEADER} 
    ${ALBEDO_EDITOR_SOURCE}
)

target_link_libraries(AlbedoEditor PRIVATE AlbedoRuntime)
set_target_properties(
    AlbedoEditor
    PROPERTIES FOLDER "${PROJECT_NAME}/Editor")
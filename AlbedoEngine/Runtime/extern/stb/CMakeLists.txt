file(GLOB STB_HEADERS "*.h")

add_library(stb INTERFACE)
target_sources(stb PRIVATE ${STB_HEADERS})
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
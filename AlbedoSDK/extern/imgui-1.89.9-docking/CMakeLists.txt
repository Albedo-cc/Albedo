set(EXT_IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB IMGUI_SOURCE CONFIGURE_DEPENDS  "${EXT_IMGUI_DIR}/*.cpp")
file(GLOB IMGUI_IMPLEMENT CONFIGURE_DEPENDS  
"${EXT_IMGUI_DIR}/backends/imgui_impl_glfw.cpp" 
"${EXT_IMGUI_DIR}/backends/imgui_impl_glfw.h"
"${EXT_IMGUI_DIR}/backends/imgui_impl_vulkan.cpp" 
"${EXT_IMGUI_DIR}/backends/imgui_impl_vulkan.h")

add_library(imgui STATIC ${IMGUI_SOURCE} ${IMGUI_IMPLEMENT})
target_link_libraries(imgui PUBLIC glfw)
target_include_directories(imgui PUBLIC ${Vulkan_INCLUDE_DIR})
target_include_directories(imgui PUBLIC $<BUILD_INTERFACE:${EXT_IMGUI_DIR}>)
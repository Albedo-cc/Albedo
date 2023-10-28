if (WIN32)
    add_library(ktx INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/bin/ktx.dll")
else()
    message(FATAL_ERROR "Sorry, only support KTX-WIN32!")
endif()

# Source
target_include_directories(ktx INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
file(GLOB_RECURSE KTX_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
source_group("" FILES ${KTX_INCLUDE})
target_sources(ktx PRIVATE ${KTX_INCLUDE})
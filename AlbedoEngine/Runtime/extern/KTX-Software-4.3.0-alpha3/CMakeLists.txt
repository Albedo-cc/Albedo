if (NOT WIN32)
    message(FATAL_ERROR "Sorry, only support KTX-WIN32!")
endif()

# Source
# target_include_directories(ktx INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
# file(GLOB_RECURSE KTX_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
# source_group("" FILES ${KTX_INCLUDE})
# target_sources(ktx PRIVATE ${KTX_INCLUDE})
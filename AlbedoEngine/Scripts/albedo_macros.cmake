if (NOT TARGET AlbedoEngine)
    message(FATAL_ERROR "Make sure that the AlbedoEngine has been built.")
endif()

macro(add_engine_dependencies new_dependencies)
    add_dependencies(AlbedoEngine new_dependencies)
endmacro()
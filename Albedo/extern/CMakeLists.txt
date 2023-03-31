# Load Libraries
add_library(AlbedoExternalLibraries INTERFACE)

# Albedo RHI
if (NOT TARGET AlbedoRHI)
    message(STATUS "\nLoading Albedo RHI (Albedo::RHI)")
    add_subdirectory(AlbedoRHI)
    target_link_libraries(AlbedoExternalLibraries INTERFACE Albedo::RHI)
endif()

# Albedo Log
if (NOT TARGET AlbedoLog)
    message(STATUS "\nLoading Albedo Log (Albedo::log)")
    add_subdirectory(AlbedoLog)
    target_link_libraries(AlbedoExternalLibraries INTERFACE Albedo::log)
endif()

# Albedo Utils
if (NOT TARGET AlbedoUtils)
    message(STATUS "\nLoading Albedo Utils (Albedo::utils)")
    add_subdirectory(AlbedoUtils)
    target_link_libraries(AlbedoExternalLibraries INTERFACE Albedo::utils)
endif()

# Eigen
if (NOT TARGET eigen)
    message(STATUS "\nLoading Eigen3 (eigen)")
    add_subdirectory(Eigen-3.4.0)
    target_link_libraries(AlbedoExternalLibraries INTERFACE eigen)
endif()

# stb_image
if (NOT TARGET stb_image)
    message(STATUS "\nLoading stb_image (stb_image)")
    add_subdirectory(stb_image-2023.1.29)
    target_link_libraries(AlbedoExternalLibraries INTERFACE stb_image)
endif()
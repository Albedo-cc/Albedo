file(GLOB EIGEN_HEADERS "Eigen/*")
list(REMOVE_ITEM ${EIGEN_HEADERS} src)

add_library(eigen INTERFACE)
target_sources(eigen PRIVATE ${EIGEN_HEADERS})
target_include_directories(eigen INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
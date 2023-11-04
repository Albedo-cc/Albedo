
# [Vulkan](https://docs.vulkan.org/spec/latest/index.html)
- vcpkg install vulkan
```CMake
find_package(Vulkan REQUIRED)
target_link_libraries(<TARGET> |PRIVATE| Vulkan::Vulkan)
```

# [VMA](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- vcpkg install vulkan-memory-allocator
```CMake
find_package(VulkanMemoryAllocator REQUIRED)
target_link_libraries(<TARGET> |PRIVATE| GPUOpen::VulkanMemoryAllocator)
```

# [GLFW](https://www.glfw.org/)
- vcpkg install glfw3
```CMake
find_package(glfw3 REQUIRED)
target_link_libraries(<TARGET> |PRIVATE| glfw)
```

# [KTX](https://github.com/KhronosGroup/KTX-Software)
- vcpkg install ktx[vulkan]
```CMake
find_package(Ktx REQUIRED)
target_link_libraries(<TARGET> |PRIVATE| INTERFACE KTX::ktx)
```

# [CGAL](https://doc.cgal.org/latest/Manual/windows.html)
- vcpkg install yasm-tool:x86-windows
- vcpkg install cgal
```CMake
find_package(CGAL REQUIRED)
target_link_libraries(<TARGET> |PRIVATE| CGAL::CGAL)
```

# [ImGui-Docking]()
- vcpkg install imgui[docking-experimental,vulkan-binding,glfw-binding]
```CMake
find_package(imgui REQUIRED)
target_link_libraries(<TARGET> |PRIVATE| imgui::imgui)
```

# [Spdlog]()
- vcpkg install spdlog
```CMake
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(<TARGET> |PRIVATE| spdlog::spdlog)
```

# [TinyglTF]()
- vcpkg install tinygltf
```CMake
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
target_include_directories(<TARGET> |PRIVATE| ${TINYGLTF_INCLUDE_DIRS})
```

# [Eigen]()
- vcpkg install eigen3
```CMake
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
target_include_directories(<TARGET> |PRIVATE| ${TINYGLTF_INCLUDE_DIRS})
```

# [Nlohmann JSON]()
- vcpkg install nlohmann-json
```CMake
find_package(nlohmann_json REQUIRED)
target_link_libraries(<TARGET> |PRIVATE| nlohmann_json::nlohmann_json)
```
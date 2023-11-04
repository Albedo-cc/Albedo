call AlbedoEngine\Extern\vcpkg\bootstrap-vcpkg.bat && \
AlbedoEngine\Extern\vcpkg\vcpkg install vulkan
::VMA
AlbedoEngine\Extern\vcpkg\vcpkg install vulkan-memory-allocator && \
::GLFW
AlbedoEngine\Extern\vcpkg\vcpkg install glfw3 && \
::KTX
AlbedoEngine\Extern\vcpkg\vcpkg install ktx[vulkan] && \
::CGAL
AlbedoEngine\Extern\vcpkg\vcpkg install yasm-tool:x86-windows && \
AlbedoEngine\Extern\vcpkg\vcpkg install cgal && \
::ImGui
AlbedoEngine\Extern\vcpkg\vcpkg install imgui[docking-experimental,vulkan-binding,glfw-binding] && \
::ImGui-Docking
AlbedoEngine\Extern\vcpkg\vcpkg install spdlog && \
::TinyglTF
AlbedoEngine\Extern\vcpkg\vcpkg install tinygltf && \
::Eigen
AlbedoEngine\Extern\vcpkg\vcpkg install eigen3 && \
::Nlohmann JSON
AlbedoEngine\Extern\vcpkg\vcpkg install nlohmann-json
#pragma once

#include <vulkan/vulkan.h>

#include <core/math/math.h>

#include <memory>
#include <optional>
#include <stdexcept>
#include <algorithm>
#include <string_view>

namespace Albedo {
namespace Runtime
{
	using namespace Albedo::Core;

	enum AssetUsageFlags
	{
		ASSET_USAGE_RENDER_BIT = 1 << 0,

		ASSET_USAGE_ALL = 0xFFFFFFFF,
	};

	class Image
	{
		friend class ImageLoader;
	public:
		int width = 0;
		int height = 0;
		int channel = 0;
		uint8_t* data = nullptr;

		size_t Size() { return static_cast<size_t>(width) * height * channel; }

		Image() = default;
		~Image();
	};

	class Model // glTF (https://github.com/SaschaWillems/Vulkan/blob/master/examples/gltfloading/gltfloading.cpp)
	{
		friend class ModelLoader;
	public:
		struct PBRParameters
		{
			std::optional<uint32_t> Base_Color_Index;
		};
		PBRParameters PBR_parameters;

	public:
		using VertexIndex = uint32_t; // VK_INDEX_TYPE_UINT32

		static VkVertexInputBindingDescription GetBindingDescription(uint32_t binding);
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescription(uint32_t binding);

		size_t GetVertexBufferSize() { return vertices.size() * sizeof(Vertex); }
		size_t GetIndexBufferSize() { return indices.size() * sizeof(VertexIndex); }

	public:
		struct Vertex
		{
			alignas(16) Vector3f position;
			alignas(16) Vector3f normal;
			alignas(16) Vector2f uv;
			alignas(16) Vector4f color;
		};

		// The following structures roughly represent the glTF scene structure
		// To keep things simple, they only contain those properties that are required for this sample

		// A primitive contains the data for a single draw call
		struct Primitive
		{
			uint32_t	first_index;
			uint32_t	index_count;
			int32_t	material_index;
		};

		// Contains the node's (optional) geometry and can be made up of an arbitrary number of primitives
		struct Mesh
		{
			std::vector<Primitive> primitives;
		};

		// A node represents an object in the glTF scene graph
		struct Node : public std::enable_shared_from_this<Node>
		{
			std::weak_ptr<Node> parent;
			std::vector<std::shared_ptr<Node>> children;
			Mesh mesh;
			Matrix4f matrix;
		};

		// A glTF material stores information in e.g. the texture that is attached to it and colors
		struct Material
		{
			Vector4f base_color_factor{ 1.0, 1.0, 1.0, 1.0 };
			uint32_t base_color_texture_index;
		};

		// A glTF texture stores a reference to the image and a sampler
		// In this sample, we are only interested in the image
		struct Texture
		{
			int32_t imageIndex;
		};

		std::vector<Vertex> vertices;
		std::vector<VertexIndex> indices;
		std::vector<Image> images;
		std::vector<Texture> textures;
		std::vector<Material> materials;
		std::vector<std::shared_ptr<Node>> nodes;
	};


}} // namespace Albedo::Runtime
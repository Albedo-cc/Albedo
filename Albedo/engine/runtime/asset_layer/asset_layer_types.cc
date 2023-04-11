#include "asset_layer_types.h"

namespace Albedo {
namespace Runtime
{
	
	std::vector<VkVertexInputAttributeDescription> Model::
		GetAttributeDescription(uint32_t binding)
	{
		return std::vector<VkVertexInputAttributeDescription>
		{
			VkVertexInputAttributeDescription
			{
				.location = 0,
				.binding = binding,
				.format = VK_FORMAT_R32G32B32_SFLOAT,
				.offset = offsetof(Model::Vertex, position)
			},
			VkVertexInputAttributeDescription
			{
				.location = 1,
				.binding = binding,
				.format = VK_FORMAT_R32G32B32_SFLOAT,
				.offset = offsetof(Model::Vertex, normal)
			},
			VkVertexInputAttributeDescription
			{
				.location = 2,
				.binding = binding,
				.format = VK_FORMAT_R32G32_SFLOAT,
				.offset = offsetof(Model::Vertex, uv)
			},
			VkVertexInputAttributeDescription
			{
				.location = 3,
				.binding = binding,
				.format = VK_FORMAT_R32G32B32A32_SFLOAT,
				.offset = offsetof(Model::Vertex, color)
			}
		};
	}

}} // namespace Albedo::Runtime
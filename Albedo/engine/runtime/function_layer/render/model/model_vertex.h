#pragma once

#include <core/math/math.h>

#include <vulkan/vulkan.h>

#include <array>

namespace Albedo {
namespace Runtime
{
	using ModelVertexIndex = uint32_t; // VK_INDEX_TYPE_UINT32
	struct ModelVertex
	{
		static const uint16_t ATTRIBUTE_COUNT = 2; // Equals to the varibles in Model Vertex
		alignas(16) Vector2f m_positon;
		alignas(16) Vector3f m_color;

		// Binding Description & Attribute Description
		static VkVertexInputBindingDescription&
			GetBindingDescription(uint32_t binding)
		{
			static VkVertexInputBindingDescription vertexInputBindingDescription
			{
				.binding = binding,
				.stride = sizeof(ModelVertex),
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
			};
			return vertexInputBindingDescription;
		}
		
		static std::array<VkVertexInputAttributeDescription, ATTRIBUTE_COUNT>&
			GetAttributeDescription(uint32_t binding)
		{
			static std::array<VkVertexInputAttributeDescription, ATTRIBUTE_COUNT>
				vertexInputAttributeDescription
			{
				VkVertexInputAttributeDescription
				{
					.location = 0,
					.binding = binding,
					.format = VK_FORMAT_R32G32_SFLOAT,
					.offset = offsetof(ModelVertex, m_positon)
				},
				VkVertexInputAttributeDescription
				{
					.location = 1,
					.binding = binding,
					.format = VK_FORMAT_R32G32B32_SFLOAT,
					.offset = offsetof(ModelVertex, m_color)
				}
			};
			return vertexInputAttributeDescription;
		}
	};

}} // namespace Albedo::Runtime
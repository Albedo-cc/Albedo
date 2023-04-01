#pragma once

#include <core/math/math.h>

#include <vulkan/vulkan.h>

#include <array>

namespace Albedo {
namespace Runtime
{
	using namespace Albedo::Core;

	using ModelVertexIndex = uint32_t; // VK_INDEX_TYPE_UINT32
	struct ModelVertex
	{
		enum AttributeDescription
		{
			attribute_description_position,
			attribute_description_color,
			attribute_description_texture_coordinate,

			MAX_ATTRIBUTE_DESCRIPTION_COUNT
		};
		alignas(16) Vector2f m_positon;
		alignas(16) Vector3f m_color;
		alignas(16) Vector2f m_texture_coord;

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
		
		static std::array<VkVertexInputAttributeDescription, MAX_ATTRIBUTE_DESCRIPTION_COUNT>&
			GetAttributeDescription(uint32_t binding)
		{
			static std::array<VkVertexInputAttributeDescription, MAX_ATTRIBUTE_DESCRIPTION_COUNT>
				vertexInputAttributeDescription
			{
				VkVertexInputAttributeDescription
				{
					.location = attribute_description_position,
					.binding = binding,
					.format = VK_FORMAT_R32G32_SFLOAT,
					.offset = offsetof(ModelVertex, m_positon)
				},
				VkVertexInputAttributeDescription
				{
					.location = attribute_description_color,
					.binding = binding,
					.format = VK_FORMAT_R32G32B32_SFLOAT,
					.offset = offsetof(ModelVertex, m_color)
				},
				VkVertexInputAttributeDescription
				{
					.location = attribute_description_texture_coordinate,
					.binding = binding,
					.format = VK_FORMAT_R32G32_SFLOAT,
					.offset = offsetof(ModelVertex, m_texture_coord)
				}
			};
			return vertexInputAttributeDescription;
		}
	};

}} // namespace Albedo::Runtime
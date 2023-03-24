#pragma once

#include "../../../core_layer/math/math.h"

#include <vulkan/vulkan.h>

#include <array>

namespace Albedo {
namespace Runtime
{
	
	struct ModelVertex
	{
		static const uint16_t ATTRIBUTE_COUNT = 2; // Equals to the varibles in Model Vertex
		Vector2 m_positon;
		Vector3 m_color;

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
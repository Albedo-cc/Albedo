#pragma once

#include <AlbedoRHI.hpp>

#include "model_vertex.h"

namespace Albedo {
namespace Runtime
{

	class Model
	{
	public:
		void Draw(RHI::CommandPool::CommandBuffer& command_buffer)
		{
			VkBuffer buffer = m_buffer;
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(command_buffer, 0, 1, &buffer, offsets);
			vkCmdDraw(command_buffer, static_cast<uint32_t>(m_vertices.size()), 1, 0, 0);
		}

	public:
		Model() = delete;
		Model(std::shared_ptr<RHI::VulkanContext> vulkan_context,
			std::vector<ModelVertex>& vertices,
			uint32_t binding) :
			m_vulkan_context{ std::move(vulkan_context) },
			m_vertices{ vertices },
			m_binding{ binding },
			m_buffer{	m_vulkan_context->m_memory_allocator->GetBuffer(
								m_vulkan_context->m_memory_allocator->
								CreateBuffer(sizeof(ModelVertex) * m_vertices.size(),
														VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
														VK_SHARING_MODE_EXCLUSIVE)) }
		{
			m_buffer.Write(m_vertices.data());
		}

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;
		std::vector<ModelVertex>& m_vertices;
		uint32_t m_binding;
		RHI::VMA::Buffer& m_buffer;
	};

}} // namespace Albedo::Runtime
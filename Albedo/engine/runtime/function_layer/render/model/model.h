#pragma once

#include <AlbedoRHI.hpp>

#include "model_vertex.h"

namespace Albedo {
namespace Runtime
{

	class Model
	{
	public:
		Matrix4f GetModelMatrix() const
		{
			Matrix4f model_matrix;
		}

		void Draw(RHI::CommandBuffer& command_buffer)
		{
			VkBuffer VBO	= *m_vertex_buffer;
			VkBuffer IBO		= *m_index_buffer;
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(command_buffer, 0, 1, &VBO, offsets);
			if (!m_indices.empty())
			{
				vkCmdBindIndexBuffer(command_buffer, IBO, 0, VK_INDEX_TYPE_UINT32);
				vkCmdDrawIndexed(command_buffer, m_indices.size(), 1, 0, 0, 0);
			}
			else vkCmdDraw(command_buffer, m_vertices.size(), 1, 0, 0);
		}

	public:
		Model() = delete;
		Model(std::shared_ptr<RHI::VulkanContext> vulkan_context,
			std::vector<ModelVertex>& vertices,
			std::vector<ModelVertexIndex>& indices,
			uint32_t binding) :
			m_vulkan_context{ std::move(vulkan_context) },
			m_vertices{ vertices },
			m_indices{ indices },
			m_binding{ binding }
		{
			// Device Local Memorys
			create_vertex_buffer();
			create_index_buffer();
		}

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;
		uint32_t m_binding;

		std::vector<ModelVertex>& m_vertices;
		std::shared_ptr<RHI::VMA::Buffer> m_vertex_buffer;
		std::vector<ModelVertexIndex>& m_indices;
		std::shared_ptr<RHI::VMA::Buffer> m_index_buffer;

	private:
		void create_vertex_buffer()
		{
			m_vertex_buffer = m_vulkan_context->m_memory_allocator->
				AllocateBuffer(sizeof(ModelVertex) * m_vertices.size(),
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
					VK_BUFFER_USAGE_TRANSFER_DST_BIT);

			auto staging_buffer = m_vulkan_context->m_memory_allocator->
				AllocateBuffer(sizeof(ModelVertex) * m_vertices.size(),
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true, true, false);
			staging_buffer->Write(m_vertices.data());

			VkBufferCopy copyinfo
			{
				.srcOffset = 0,
				.dstOffset = 0,
				.size = staging_buffer->Size()
			};

			auto commandBuffer = m_vulkan_context->GetOneTimeCommandBuffer();
			commandBuffer->Begin();
			vkCmdCopyBuffer(*commandBuffer, *staging_buffer, *m_vertex_buffer, 1, &copyinfo);
			commandBuffer->End();
			commandBuffer->Submit(true); // Must wait for transfer operation
		}

		void create_index_buffer()
		{
			m_index_buffer = m_vulkan_context->m_memory_allocator->
				AllocateBuffer(sizeof(ModelVertexIndex) * m_indices.size(),
					VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
					VK_BUFFER_USAGE_TRANSFER_DST_BIT);

			auto staging_buffer = m_vulkan_context->m_memory_allocator->
				AllocateBuffer(sizeof(ModelVertexIndex) * m_indices.size(),
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true, true, false);
			staging_buffer->Write(m_indices.data());

			VkBufferCopy copyinfo
			{
				.srcOffset = 0,
				.dstOffset = 0,
				.size = staging_buffer->Size()
			};

			auto commandBuffer = m_vulkan_context->GetOneTimeCommandBuffer();
			commandBuffer->Begin();
			vkCmdCopyBuffer(*commandBuffer, *staging_buffer, *m_index_buffer, 1, &copyinfo);
			commandBuffer->End();
			commandBuffer->Submit(true); // Must wait for transfer operation
		}
	};

}} // namespace Albedo::Runtime
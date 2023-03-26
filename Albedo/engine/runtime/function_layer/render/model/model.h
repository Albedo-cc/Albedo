#pragma once

#include <AlbedoRHI.hpp>

#include "model_vertex.h"

namespace Albedo {
namespace Runtime
{

	class Model
	{
	public:
		void Draw(RHI::CommandBuffer& command_buffer)
		{
			VkBuffer buffer = *m_buffer;
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
			m_buffer{// Device Local Memory
								m_vulkan_context->m_memory_allocator->
								CreateBuffer(sizeof(ModelVertex) * m_vertices.size(),
														VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
														VK_BUFFER_USAGE_TRANSFER_DST_BIT) }
		{
			auto staging_buffer = m_vulkan_context->m_memory_allocator->
									CreateBuffer(sizeof(ModelVertex) * m_vertices.size(),
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
			vkCmdCopyBuffer(*commandBuffer, *staging_buffer, *m_buffer, 1, &copyinfo);
			commandBuffer->End();
			commandBuffer->Submit(true);
		}

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;
		std::vector<ModelVertex>& m_vertices;
		uint32_t m_binding;
		std::shared_ptr<RHI::VMA::Buffer> m_buffer;
	};

}} // namespace Albedo::Runtime
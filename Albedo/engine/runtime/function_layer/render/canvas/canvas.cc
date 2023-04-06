#include "canvas.h"

namespace Albedo {
namespace Runtime
{
	
	void Canvas::Render(SID scene_id, RHI::GraphicsPipeline* pipeline)
	{
		pipeline->Bind(m_command_buffer);
		// vkCmdBindDescriptorSets
		auto& scene = m_scenes[scene_id];
	}

	void Canvas::LoadScene(std::shared_ptr<SModel> scene_data)
	{
		auto& new_scene = m_scenes.emplace_back();

		// Create VBO & IBO
		new_scene.vertex_buffer = m_vulkan_context->m_memory_allocator->
													AllocateBuffer(scene_data->GetVertexBufferSize(),
													VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
													VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		auto staging_buffer_vertex = m_vulkan_context->m_memory_allocator->AllocateStagingBuffer(new_scene.vertex_buffer->Size());
		auto vertex_buffer_copy_info = new_scene.vertex_buffer->GetCopyInfo();

		new_scene.index_buffer = m_vulkan_context->m_memory_allocator->
													AllocateBuffer(scene_data->GetIndexBufferSize(),
													VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
													VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		auto staging_buffer_index = m_vulkan_context->m_memory_allocator->AllocateStagingBuffer(new_scene.vertex_buffer->Size());
		auto index_buffer_copy_info = new_scene.index_buffer->GetCopyInfo();

		// Create Textures
		auto sampler = m_vulkan_context->CreateSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT);
		new_scene.textures.resize(scene_data->images.size());
		for (size_t i = 0; i < scene_data->images.size(); ++i)
		{
			auto& image = scene_data->images[i];
			auto& texture = new_scene.textures[i];
			texture = m_vulkan_context->m_memory_allocator->
							AllocateImage(VK_IMAGE_ASPECT_COLOR_BIT,
							VK_IMAGE_USAGE_SAMPLED_BIT,
							image.width, image.height, image.channel,
							VK_FORMAT_R8G8B8A8_SRGB);
			texture->BindSampler(sampler); // Q: All images in gtTF are used as textures(with a sampler)?
		}
		
		auto commandBuffer = m_vulkan_context->GetOneTimeCommandBuffer();
		commandBuffer->Begin();
		{
			for (size_t i = 0; i < scene_data->images.size(); ++i)
				new_scene.textures[i]->WriteCommand(commandBuffer, scene_data->images[i].data);
			vkCmdCopyBuffer(*commandBuffer, *staging_buffer_vertex, *new_scene.vertex_buffer, 1, &vertex_buffer_copy_info);
			vkCmdCopyBuffer(*commandBuffer, *staging_buffer_index, *new_scene.index_buffer, 1, &index_buffer_copy_info);
		}
		commandBuffer->End();
		commandBuffer->Submit(true);

		// Allocate Descriptor Sets

	}

}} // namespace Albedo::Runtime
#include "scene.h"

#include "palette.h"

namespace Albedo {
namespace Runtime
{
	void Scene::Sketch(std::shared_ptr<Model> model)
	{
		textures = model->textures;
		materials = model->materials;
		nodes = model->nodes;

		// Load Data from Scene
		{
			// Vertex Buffer
			vertices = m_vulkan_context->m_memory_allocator->
				AllocateBuffer(model->GetVertexBufferSize(),
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
					VK_BUFFER_USAGE_TRANSFER_DST_BIT);
			auto staging_buffer_vertex = m_vulkan_context->m_memory_allocator->
				AllocateStagingBuffer(model->GetVertexBufferSize());
			staging_buffer_vertex->Write(model->vertices.data());

			// Index Buffer
			indices = m_vulkan_context->m_memory_allocator->
				AllocateBuffer(model->GetIndexBufferSize(),
					VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
					VK_BUFFER_USAGE_TRANSFER_DST_BIT);
			auto staging_buffer_index = m_vulkan_context->m_memory_allocator->
				AllocateStagingBuffer(model->GetIndexBufferSize());
			staging_buffer_index->Write(model->indices.data());

			// Images
			auto sampler = m_vulkan_context->CreateSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT);
			std::vector<std::shared_ptr<RHI::VMA::Buffer>> image_buffers(model->images.size());
			images.resize(model->images.size());
			for (size_t i = 0; i < model->images.size(); ++i)
			{
				auto& current_image = model->images[i];
				images[i] = m_vulkan_context->m_memory_allocator->
					AllocateImage(VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_SAMPLED_BIT,
						current_image.width, current_image.height, current_image.channel, VK_FORMAT_R8G8B8A8_SRGB);
				images[i]->BindSampler(sampler);

				image_buffers[i] = m_vulkan_context->m_memory_allocator->AllocateStagingBuffer(images[i]->Size());
				image_buffers[i]->Write(current_image.data);
			}


			// Submit
			auto commandBuffer = m_vulkan_context->
				CreateOneTimeCommandBuffer(m_vulkan_context->m_device_queue_family_graphics);
			commandBuffer->Begin();
			staging_buffer_vertex->CopyCommand(commandBuffer, vertices);
			staging_buffer_index->CopyCommand(commandBuffer, indices);
			for (size_t i = 0; i < images.size(); ++i)
			{
				images[i]->WriteAndTransitionCommand(commandBuffer, image_buffers[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			}
			commandBuffer->End();
			commandBuffer->Submit(true); // Must wait for transfer operation

			// Material Descriptor Set
			m_descriptor_set_materials.resize(materials.size());
			for (size_t i = 0; i < materials.size(); ++i)
			{
				m_descriptor_set_materials[i] = m_vulkan_context->CreateDescriptorSet(Palette::SET1_Materials_Layout);

				if (materials[i].base_color_texture_index.has_value())
				{
					Palette::SetupPBRBaseColor(m_descriptor_set_materials[i],
						images[textures[materials[i].base_color_texture_index.value()].imageIndex]);
				}
				else log::warn("Current Primitive has no PBR Base Color Textures!");
			}
		}
	}

}} // namespace Albedo::Runtime
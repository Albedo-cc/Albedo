#include "easel.h"

#include <runtime/asset_layer/asset_manager.h>

namespace Albedo {
namespace Runtime
{
	constexpr size_t MAX_DESCRIPTOR_POOL_SET_COUNT
	{ Easel::MAX_CANVAS_COUNT * Easel::MAX_SET_COUNT };

	constexpr size_t MAX_DESCRIPTOR_POOL_SIZE_UBO
	{ Easel::MAX_CANVAS_COUNT * Easel::MAX_UNIFORM_BUFFER_COUNT };

	constexpr size_t MAX_DESCRIPTOR_POOL_SIZE_TEXTURE
	{ Easel::MAX_CANVAS_COUNT * Easel::MAX_TEXTURE_COUNT };

	Easel::Easel(std::shared_ptr<RHI::VulkanContext> vulkan_context) :
		m_vulkan_context{ std::move(vulkan_context) }
	{
		// Command Pool
		m_command_pool	=	m_vulkan_context->CreateCommandPool(
												m_vulkan_context->m_device_queue_graphics.value(),
												VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		// Descriptor Pool
		m_descriptor_pool	= m_vulkan_context->CreateDescriptorPool(std::vector<VkDescriptorPoolSize>
		{
			VkDescriptorPoolSize // Uniform Buffers
			{
				.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = MAX_DESCRIPTOR_POOL_SIZE_UBO,
			},
				VkDescriptorPoolSize // Image Samplers
			{
				.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = MAX_DESCRIPTOR_POOL_SIZE_TEXTURE 
			}
		}, MAX_DESCRIPTOR_POOL_SET_COUNT);

		// Canvases
		m_canvases.resize(MAX_CANVAS_COUNT);
		for (size_t i = 0; i < MAX_CANVAS_COUNT; ++i)
		{
			auto& canvas = m_canvases[i];
			canvas.m_command_buffer = m_command_pool->AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
			// Sync Meta
			canvas.syncmeta.fence_in_flight = m_vulkan_context->CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
			canvas.syncmeta.semaphore_image_available = m_vulkan_context->CreateSemaphore(0x0);
			canvas.syncmeta.semaphore_render_finished = m_vulkan_context->CreateSemaphore(0x0);
		}
	}

	Canvas& Easel::GetCanvas() throw (RHI::VulkanContext::swapchain_error)
	{
		auto& canvas = m_canvases[m_current_canvas];
		// Sync
		canvas.syncmeta.fence_in_flight->Wait();
		m_vulkan_context->NextSwapChainImageIndex(*canvas.syncmeta.semaphore_image_available, VK_NULL_HANDLE);
		canvas.syncmeta.fence_in_flight->Reset();

		return canvas;
	}

	void Easel::PresentCanvas(bool switch_canvas = true) throw (RHI::VulkanContext::swapchain_error)
	{
		auto& canvas = m_canvases[m_current_canvas];

		m_vulkan_context->PresentSwapChain(*canvas.syncmeta.semaphore_render_finished);

		m_current_canvas = (m_current_canvas + switch_canvas) % MAX_CANVAS_COUNT;
	}

	void Easel::SetScene(std::shared_ptr<SModel> scene_data)
	{
			// Create VBO & IBO
			m_scene.vertices = m_vulkan_context->m_memory_allocator->
														AllocateBuffer(scene_data->GetVertexBufferSize(),	
																					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
																					VK_BUFFER_USAGE_TRANSFER_DST_BIT);
			auto staging_buffer_vertex = m_vulkan_context->m_memory_allocator->AllocateStagingBuffer(m_scene.vertices->Size());
			auto vertices_copy_info = m_scene.vertices->GetCopyInfo();

			m_scene.indices = m_vulkan_context->m_memory_allocator->
														AllocateBuffer(scene_data->GetIndexBufferSize(),
																					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
																					VK_BUFFER_USAGE_TRANSFER_DST_BIT);
			auto staging_buffer_index = m_vulkan_context->m_memory_allocator->AllocateStagingBuffer(m_scene.vertices->Size());
			auto indices_copy_info = m_scene.indices->GetCopyInfo();

			// Create Textures
			auto sampler = m_vulkan_context->CreateSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT);
			m_scene.textures.resize(scene_data->images.size());
			for (size_t i = 0; i < scene_data->images.size(); ++i)
			{
				auto& image = scene_data->images[i];
				auto& texture = m_scene.textures[i];
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
					m_scene.textures[i]->WriteCommand(commandBuffer, scene_data->images[i].data);
				vkCmdCopyBuffer(*commandBuffer, *staging_buffer_vertex, *m_scene.vertices, 1, &vertices_copy_info);
				vkCmdCopyBuffer(*commandBuffer, *staging_buffer_index, *m_scene.indices, 1, &indices_copy_info);
			}
			commandBuffer->End();
			commandBuffer->Submit(true);

			// Allocate Descriptor Sets
	}

}} // namespace Albedo::Runtime
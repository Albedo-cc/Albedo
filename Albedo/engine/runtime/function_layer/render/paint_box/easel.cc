#include "easel.h"

#include <runtime/asset_layer/asset_manager.h>

namespace Albedo {
namespace Runtime
{
	constexpr size_t MAX_DESCRIPTOR_POOL_SET_COUNT
	{ Easel::MAX_CANVAS_COUNT * Palette::MAX_SET_COUNT };

	constexpr size_t MAX_DESCRIPTOR_POOL_SIZE_UBO
	{ Easel::MAX_CANVAS_COUNT * Palette::MAX_UNIFORM_BUFFER_COUNT };

	constexpr size_t MAX_DESCRIPTOR_POOL_SIZE_TEXTURE
	{ Easel::MAX_CANVAS_COUNT * Palette::MAX_TEXTURE_COUNT };

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
			canvas.command_buffer = m_command_pool->AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
			// Descriptor Sets
			canvas.palette.Initialize(m_vulkan_context, m_descriptor_pool);
			// Sync Meta
			canvas.syncmeta.fence_in_flight = m_vulkan_context->CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
			canvas.syncmeta.semaphore_image_available = m_vulkan_context->CreateSemaphore(0x0);
			canvas.syncmeta.semaphore_render_finished = m_vulkan_context->CreateSemaphore(0x0);
		}
	}

	void Easel::SetupTheScene(std::shared_ptr<Model> scene)
	{
		m_scene = std::make_unique<Scene>();

		m_scene->textures = scene->textures;
		m_scene->materials = scene->materials;
		m_scene->nodes = scene->nodes;

		// Load Data from Scene
		{
			// Vertex Buffer
			auto& vertex_buffer = scene->vertices;
			m_scene->vertices = m_vulkan_context->m_memory_allocator->
				AllocateBuffer(scene->GetVertexBufferSize(),
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
					VK_BUFFER_USAGE_TRANSFER_DST_BIT);
			auto staging_buffer_vertex = m_vulkan_context->m_memory_allocator->
				AllocateStagingBuffer(scene->GetVertexBufferSize());
			staging_buffer_vertex->Write(vertex_buffer.data());

			// Index Buffer
			auto& index_buffer = scene->indices;
			m_scene->indices = m_vulkan_context->m_memory_allocator->
				AllocateBuffer(scene->GetIndexBufferSize(),
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
					VK_BUFFER_USAGE_TRANSFER_DST_BIT);
			auto staging_buffer_index = m_vulkan_context->m_memory_allocator->
				AllocateStagingBuffer(scene->GetIndexBufferSize());
			staging_buffer_index->Write(index_buffer.data());

			// Images
			static  auto sampler = m_vulkan_context->CreateSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT);
			auto& images = scene->images;
			for (const auto& image : images)
			{
				auto texture = m_vulkan_context->m_memory_allocator->
					AllocateImage(VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_SAMPLED_BIT,
						image.width, image.height, image.channel, VK_FORMAT_R8G8B8A8_SRGB);
				texture->BindSampler(sampler);
				m_scene->images.emplace_back(texture);
			}

			auto commandBuffer = m_vulkan_context->GetOneTimeCommandBuffer();
			commandBuffer->Begin();
			vkCmdCopyBuffer(*commandBuffer, *staging_buffer_vertex, *m_scene->vertices, 1, &staging_buffer_vertex->GetCopyInfo());
			vkCmdCopyBuffer(*commandBuffer, *staging_buffer_index, *m_scene->indices, 1, &staging_buffer_index->GetCopyInfo());
			for (size_t i = 0; i < m_scene->images.size(); ++i) m_scene->images[i]->WriteCommand(commandBuffer, images[i].data);
			commandBuffer->End();
			commandBuffer->Submit(true); // Must wait for transfer operation
		}
		
		// Update Descriptor Sets
		{
			for (auto& canvas : m_canvases)
			{
				for (auto& image : m_scene->images)
				{
					canvas.palette.SetupPBRBaseColor(image);
				}
			}
		} // End Update Descriptor Sets
	}

	Canvas& Easel::WaitCanvas() throw (RHI::VulkanContext::swapchain_error)
	{
		auto& canvas = m_canvases[m_current_canvas];
		// Sync
		canvas.syncmeta.fence_in_flight->Wait();
		m_vulkan_context->NextSwapChainImageIndex(*canvas.syncmeta.semaphore_image_available, VK_NULL_HANDLE);
		canvas.syncmeta.fence_in_flight->Reset();

		return canvas;
	}

	void Easel::PresentCanvas(bool switch_canvas/* = true*/) throw (RHI::VulkanContext::swapchain_error)
	{
		auto& canvas = m_canvases[m_current_canvas];

		m_vulkan_context->PresentSwapChain(*canvas.syncmeta.semaphore_render_finished);

		m_current_canvas = (m_current_canvas + switch_canvas) % MAX_CANVAS_COUNT;
	}

}} // namespace Albedo::Runtime
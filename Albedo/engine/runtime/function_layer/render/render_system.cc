#include "render_system.h"

#include "render_pass/forward_rendering/forward_render_pass.h"

namespace Albedo {
namespace Runtime
{

	RenderSystem::RenderSystem(std::weak_ptr<WindowSystem> window_system) :
		m_vulkan_context{ RHI::VulkanContext::Create(window_system.lock()->GetWindow()) },
		m_window_system{ std::move(window_system) },
		m_camera{ m_vulkan_context }
	{
		time::StopWatch timer{};
		// Async Loading Assets
		auto image_futures = begin_loading_images();
		auto model_futures = AssetManager::instance().AsyncLoadModel("Cube/Cube.gltf", ASSET_USAGE_RENDER_BIT);
		// Initialize Render System Context
		RenderSystemContext::Initialize(m_vulkan_context);

		// Render Passes
		create_render_passes();

		// Load Assets
		load_models();
		end_loading_images(image_futures);
		auto model = model_futures->WaitResult();
		log::info("Render System Initialization Time : {} ms", timer.split().microseconds());
	}

	void RenderSystem::wait_for_next_image_index(FrameState& current_frame_state)
	{
		if (m_window_system.lock()->IsResized(true)) 
			throw RHI::VulkanContext::swapchain_error();

		m_vulkan_context->NextSwapChainImageIndex(
			*current_frame_state.m_semaphore_image_available, VK_NULL_HANDLE);
	}

	void RenderSystem::create_render_passes()
	{
		m_render_passes.clear();
		m_render_passes.resize(MAX_RENDER_PASS_COUNT);
		m_render_passes[render_pass_forward] = std::make_unique<ForwardRenderPass>(m_vulkan_context);
	}

	void RenderSystem::load_models()
	{
		static std::vector<ModelVertexIndex>
		triangle_indices
		{
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
		};

		static std::vector<ModelVertex> 
		triangle_vertices
		{	// [ X		Y		  Z ]		[ R		G		  B ]	  [ U		   V]
			{ {-0.5f, -0.5f, 0.0f},	{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },
			{ {0.5f, -0.5f, 0.0f},		{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
			{ {0.5f, 0.5f, 0.0f},		{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },
			{ {-0.5f, 0.5f, 0.0f},		{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },

			{ {-0.5f, -0.5f, -0.5f},	{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },
			{ {0.5f, -0.5f, -0.5f},	{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
			{ {0.5f, 0.5f, -0.5f},		{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },
			{ {-0.5f, 0.5f, -0.5f},	{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} }
		};
		m_models.emplace_back(m_vulkan_context, triangle_vertices, triangle_indices, 0);
	}

	std::vector<std::shared_ptr<ImageFuture>> RenderSystem::
		begin_loading_images()
	{
		std::vector<std::shared_ptr<ImageFuture>> image_tasks(1);
		image_tasks[0] = AssetManager::instance().AsyncLoadTexture2D("watch_tower_512x512.png");
		return image_tasks;
	}

	void RenderSystem::end_loading_images(std::vector<std::shared_ptr<ImageFuture>> image_futures)
	{
		auto sampler = m_vulkan_context->CreateSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT);
		
		for (auto& image_future : image_futures)
		{
			auto image_data = image_future->WaitResult();
			m_image = m_vulkan_context->m_memory_allocator->
				AllocateImage(VK_IMAGE_ASPECT_COLOR_BIT,
					VK_IMAGE_USAGE_SAMPLED_BIT,
					image_data->width, image_data->height, image_data->channel,
					VK_FORMAT_R8G8B8A8_SRGB);
			m_image->Write(image_data->data);
			m_image->BindSampler(sampler);
		}
	}

	void RenderSystem::handle_window_resize()
	{
		log::warn("Window Resized!");
		m_vulkan_context->RecreateSwapChain();
		create_render_passes();		 // Recreate Render Passes
		m_camera.GetViewingMatrix(true); // Update
	}

}} // namespace Albedo::Runtime
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
		// Initialize Render System Context
		RenderSystemContext::Initialize(m_vulkan_context);

		// Render Passes
		create_render_passes();
		// Load Assets
		load_models();
		load_images();
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

	void RenderSystem::load_images()
	{
		auto sampler = m_vulkan_context->CreateSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT);

		auto image_data = ImageLoader::Load("resource/image/watch_tower_512x512.png");
		m_image  = m_vulkan_context->m_memory_allocator->
								AllocateImage(	VK_IMAGE_ASPECT_COLOR_BIT,
															VK_IMAGE_USAGE_SAMPLED_BIT,
															image_data->width, image_data->height, image_data->channel,
															VK_FORMAT_R8G8B8A8_SRGB);
		m_image->Write(image_data->data);
		m_image->BindSampler(sampler);
	}

	void RenderSystem::handle_window_resize()
	{
		log::warn("Window Resized!");
		m_vulkan_context->RecreateSwapChain();
		create_render_passes();		 // Recreate Render Passes

		for (auto& frame_state : RenderSystemContext::m_frame_states) // Recreate Uniform Buffer
		{
			frame_state.m_uniform_buffer = m_vulkan_context->m_memory_allocator->AllocateBuffer
			(sizeof(UniformBuffer), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, true, true, false, true); // Persistent Memory
		}
		m_camera.GetViewingMatrix(true); // Update
	}

}} // namespace Albedo::Runtime
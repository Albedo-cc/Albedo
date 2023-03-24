#include "render_system.h"

#include "render_pass/forward_rendering/forward_render_pass.h"

namespace Albedo {
namespace Runtime
{

	RenderSystem::RenderSystem(std::weak_ptr<WindowSystem> window_system) :
		m_vulkan_context{ std::make_shared<RHI::VulkanContext>(window_system.lock()->GetWindow()) },
		m_window_system{ std::move(window_system) },
		// Command Pool
		m_command_pool_reset{
				m_vulkan_context,
				m_vulkan_context->m_device_queue_graphics.value(),
				VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT },
		m_framebuffer_pool{ m_vulkan_context }
	{
		// Render Passes
		create_render_passes();
		// Framebuffers
		create_framebuffer_pool();
		// Frame States
		create_frame_states();
	}

	uint32_t RenderSystem::wait_for_next_image_index(FrameState& current_frame_state)
	{
		m_vulkan_context->NextSwapChainImageIndex(
			*current_frame_state.m_semaphore_image_available, VK_NULL_HANDLE);

		if (m_window_system.lock()->IsResized(true)) 
			throw RHI::VulkanContext::swapchain_error();

		return m_vulkan_context->m_swapchain_current_image_index;
	}

	void RenderSystem::create_framebuffer_pool()
	{
		m_framebuffer_pool = RHI::FramebufferPool(m_vulkan_context);

		for (size_t i = 0; i < m_vulkan_context->m_swapchain_imageviews.size(); ++i)
		{
			std::vector<VkImageView> attachments{ m_vulkan_context->m_swapchain_imageviews[i] };

			VkFramebufferCreateInfo framebufferCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass = *(m_render_passes[render_pass_forward]),
				.attachmentCount = static_cast<uint32_t>(attachments.size()),
				.pAttachments = attachments.data(),
				.width = m_vulkan_context->m_swapchain_current_extent.width,
				.height = m_vulkan_context->m_swapchain_current_extent.height,
				.layers = 1
			};
			m_framebuffer_pool.AllocateFramebuffer(framebufferCreateInfo);
		}
	}

	void RenderSystem::create_frame_states()
	{
		m_frame_states.reserve(MAX_FRAME_IN_FLIGHT);
		for (int i = 0; i < MAX_FRAME_IN_FLIGHT; ++i)
		{
			auto token = m_command_pool_reset.AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
			auto& state = m_frame_states.emplace_back(
				FrameState{ .m_command_buffer = m_command_pool_reset[token] });
			state.m_fence_in_flight = std::make_unique<RHI::Fence>(m_vulkan_context, VK_FENCE_CREATE_SIGNALED_BIT);
			state.m_semaphore_image_available	= std::make_unique<RHI::Semaphore>(m_vulkan_context, 0x0);
			state.m_semaphore_render_finished	= std::make_unique<RHI::Semaphore>(m_vulkan_context, 0x0);
		}
	}

	void RenderSystem::create_render_passes()
	{
		m_render_passes.clear();
		m_render_passes.resize(MAX_RENDER_PASS_COUNT);
		m_render_passes[render_pass_forward] = std::make_unique<ForwardRenderPass>(m_vulkan_context);
	}

	void RenderSystem::load_models()
	{
		static std::vector<ModelVertex> 
		triangle_vertices
		{
			{ {0.0f, -0.5f} ,{1.0f, 0.0f, 0.0f} },
			{ {0.5f, 0.5f} ,{0.0f, 1.0f, 0.0f} },
			{ {-0.5f, 0.5f} ,{0.0f, 0.0f, 1.0f} }
		};
		m_models.emplace_back(m_vulkan_context, triangle_vertices, 0);
	}

	void RenderSystem::handle_window_resize()
	{
		log::warn("Window Resized!");
		m_vulkan_context->RecreateSwapChain();
		create_framebuffer_pool(); // Recreate Framebuffers
		create_render_passes();		 // Recreate Render Passes
	}

	uint32_t RenderSystem::FrameState::GetCurrentFrame(bool increase/* = false*/)
	{
		static uint32_t current_frame{ 0 };
		if (increase) current_frame = (current_frame + 1) % MAX_FRAME_IN_FLIGHT;
		return current_frame;
	}

}} // namespace Albedo::Runtime
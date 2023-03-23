#pragma once

#include <AlbedoRHI.hpp>

#include "../window/window_system.h"

#include <memory>

namespace Albedo {
namespace Runtime
{

	class RenderSystem
	{
		static const int MAX_FRAME_IN_FLIGHT = 3;
		struct FrameState // [MAX_FRAME_IN_FLIGHT]
		{
			static int32_t	GetCurrentFrame();
			std::unique_ptr<RHI::Fence>			m_fence_in_flight;
			std::unique_ptr<RHI::Semaphore>	m_semaphore_image_available;
			std::unique_ptr<RHI::Semaphore>	m_semaphore_render_finished;
			RHI::CommandPool::CommandBuffer m_command_buffer;
		};
	public:
		RenderSystem() = delete;
		RenderSystem(std::weak_ptr<WindowSystem> window_system);

		void Update()
		{
			// Wait for previous frame
			auto& current_frame_state = m_frame_states[FrameState::GetCurrentFrame()];
			current_frame_state.m_fence_in_flight->Wait();
			// Wait for next image and handle window resize
			if (wait_for_next_image_index(current_frame_state))
			{
				current_frame_state.m_fence_in_flight->Reset();
				auto next_image_index = m_vulkan_context->m_swapchain_current_image_index;

				current_frame_state.m_command_buffer.Begin(0, 0);
				for (auto& render_pass : m_render_passes)
				{
					render_pass->Begin(current_frame_state.m_command_buffer, m_framebuffer_pool->GetFramebuffer(next_image_index));
					render_pass->Render(current_frame_state.m_command_buffer);
					render_pass->End(current_frame_state.m_command_buffer);
				}
				current_frame_state.m_command_buffer.End();

				current_frame_state.m_command_buffer.Submit(
					0, *current_frame_state.m_fence_in_flight,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					{ *current_frame_state.m_semaphore_image_available },
					{ *current_frame_state.m_semaphore_render_finished });

				m_vulkan_context->PresentSwapChain(*current_frame_state.m_semaphore_render_finished);
			}
		}

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context; // Make sure that this context will be released at last.
		std::weak_ptr<WindowSystem> m_window_system;

		enum RenderPasses
		{
			render_pass_forward,
			
			MAX_RENDER_PASS_COUNT
		};
		std::vector<std::unique_ptr<RHI::RenderPass>> m_render_passes;
		std::unique_ptr<RHI::CommandPool	>		m_command_pool_reset;
		std::unique_ptr<RHI::FramebufferPool>	m_framebuffer_pool;

		std::vector<FrameState>								m_frame_states;

	private:
		bool wait_for_next_image_index(FrameState& current_frame_state);

		void create_framebuffer_pool();
		void create_frame_states();
		void create_render_passes();
	};

}} // namespace Albedo::Runtime
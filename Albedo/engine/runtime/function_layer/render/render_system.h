#pragma once

#include "render_system_types.h"

#include "model/model.h"
#include "../window/window_system.h"

#include <memory>

namespace Albedo {
namespace Runtime
{

	class RenderSystem
	{
	public:
		RenderSystem() = delete;
		RenderSystem(std::weak_ptr<WindowSystem> window_system);

		void Update()
		{
			// Wait for previous frame
			auto& current_frame_state = m_frame_states[FrameState::GetCurrentFrame()];
			current_frame_state.m_fence_in_flight->Wait();
			try
			{
				auto next_image_index = wait_for_next_image_index(current_frame_state);

				current_frame_state.m_fence_in_flight->Reset();

				auto& current_commandbuffer = current_frame_state.m_command_buffer;
				auto& current_framebuffer = m_framebuffer_pool->GetFramebuffer(next_image_index);

				current_commandbuffer->Begin();
				{
					for (auto& render_pass : m_render_passes)
					{
						render_pass->Begin(current_commandbuffer, current_framebuffer);
						render_pass->Render(current_commandbuffer);
						for (auto& model : m_models) model.Draw(*current_commandbuffer);
						render_pass->End(current_commandbuffer);
					}
				}
				current_commandbuffer->End();

				current_commandbuffer->Submit(false,
					*current_frame_state.m_fence_in_flight,
					{ *current_frame_state.m_semaphore_image_available },
					{ *current_frame_state.m_semaphore_render_finished },
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

				m_vulkan_context->PresentSwapChain(*current_frame_state.m_semaphore_render_finished);

				FrameState::GetCurrentFrame(true); // Next Frame
			}
			catch (RHI::VulkanContext::swapchain_error& swapchian_recreation)
			{
				handle_window_resize();
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
		std::shared_ptr<RHI::CommandPool	>		m_command_pool_resetable;
		std::shared_ptr<RHI::CommandPool>		m_command_pool_transient;
		std::shared_ptr<RHI::FramebufferPool>	m_framebuffer_pool;

		std::vector<FrameState> m_frame_states;
		
		std::vector<Model> m_models;

	private:
		uint32_t wait_for_next_image_index(FrameState& current_frame_state);

		void create_framebuffer_pool();
		void create_frame_states();
		void create_render_passes();

		void load_models();

		void handle_window_resize();
	};

}} // namespace Albedo::Runtime
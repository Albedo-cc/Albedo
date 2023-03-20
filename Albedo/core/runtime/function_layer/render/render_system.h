#pragma once

#include <AlbedoRHI.hpp>

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
			m_fence_in_flight.Wait();
			// Wait for next image and handle window resize
			wait_for_next_image_index();
			auto next_image_index = m_vulkan_context->m_swapchain_current_image_index;

			static auto& primary_command_buffer = m_command_pool_reset[m_command_pool_reset.AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY)];

			primary_command_buffer.Begin(0, 0);
			for (auto& render_pass : m_render_passes)
			{
				render_pass->Begin(primary_command_buffer, m_framebuffer_pool[next_image_index]);
				render_pass->Render(primary_command_buffer);
				render_pass->End(primary_command_buffer);
			}
			primary_command_buffer.End();

			primary_command_buffer.Submit(
				0, m_fence_in_flight,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				{ m_semaphore_image_available },
				{ m_semaphore_render_finished });

			m_vulkan_context->PresentSwapChain(m_semaphore_render_finished);
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
		RHI::CommandPool		m_command_pool_reset;
		RHI::FramebufferPool	m_framebuffer_pool;

		RHI::Semaphore m_semaphore_image_available;
		RHI::Semaphore m_semaphore_render_finished;
		RHI::Fence			  m_fence_in_flight;

	private:
		void wait_for_next_image_index();

		void create_framebuffer_pool();
		void create_render_passes();
	};

}} // namespace Albedo::Runtime
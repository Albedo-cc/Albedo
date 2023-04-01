#pragma once

#include "render_system_context.h"

#include "model/model.h"
#include "camera/camera.h"
#include <core/math/math.h>
#include <runtime/resource_layer/image/image_loader.h>
#include <runtime/function_layer/window/window_system.h>

#include <AlbedoTime.hpp>

#include <memory>
#include <iostream>

namespace Albedo {
namespace Runtime
{

	class RenderSystem
	{
		enum RenderPasses
		{
			render_pass_forward,

			MAX_RENDER_PASS_COUNT
		};
	public:
		RenderSystem() = delete;
		RenderSystem(std::weak_ptr<WindowSystem> window_system);
		~RenderSystem() { m_vulkan_context->WaitDeviceIdle(); RenderSystemContext::Stop(); }

		void Update()
		{
			// Wait for previous frame
			auto& current_frame_state = RenderSystemContext::GetCurrentFrameState();
			current_frame_state.m_fence_in_flight->Wait();
			try
			{
				auto next_image_index = wait_for_next_image_index(current_frame_state);

				current_frame_state.m_fence_in_flight->Reset();

				auto& current_commandbuffer = current_frame_state.m_command_buffer;
				auto& current_framebuffer = m_framebuffer_pool->GetFramebuffer(next_image_index);

				static UniformBuffer UBO;
				static time::StopWatch timer{};

				UBO.matrix_model = make_rotation_matrix(WORLD_AXIS_Z, 0.1 * ONE_RADIAN * timer.split().milliseconds());
				UBO.matrix_view = m_camera.GetViewMatrix();//m_camera.GetViewingMatrix();
				UBO.matrix_projection = m_camera.GetProjectionMatrix();

				current_frame_state.m_uniform_buffer->Write(&UBO);
				current_frame_state.m_global_descriptor_set->WriteBuffer(
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
					RenderSystemContext::global_descriptor_set_binding_matrics,
					current_frame_state.m_uniform_buffer);

				current_frame_state.m_global_descriptor_set->WriteImage(
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					RenderSystemContext::global_descriptor_set_binding_textures,
					m_image);

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

				RenderSystemContext::SwitchToNextFrame();
			}
			catch (RHI::VulkanContext::swapchain_error& swapchian_recreation)
			{
				handle_window_resize();
			}
		}

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context; // Make sure that vulkan context will be released at last.
		std::weak_ptr<WindowSystem> m_window_system;
		Camera m_camera;

		std::vector<Model> m_models;
		std::shared_ptr<RHI::VMA::Image> m_image;

		std::vector<std::unique_ptr<RHI::RenderPass>> m_render_passes;
		std::shared_ptr<RHI::FramebufferPool>	m_framebuffer_pool;

	private:
		uint32_t wait_for_next_image_index(FrameState& current_frame_state);

		void create_framebuffer_pool();
		void create_render_passes();

		void load_models();
		void load_images();

		void handle_window_resize();
	};

}} // namespace Albedo::Runtime
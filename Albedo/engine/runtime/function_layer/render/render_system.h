#pragma once
#include <AlbedoTime.hpp>

#include "render_system_context.h"

#include "model/model.h"
#include "camera/camera.h"
#include <core/math/math.h>
#include <runtime/asset_layer/asset_manager.h>
#include <runtime/function_layer/window/window_system.h>

#include "render_pass/forward_rendering/forward_render_pass.h"

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
		~RenderSystem() { m_vulkan_context->WaitDeviceIdle(); }

		void Update()
		{
			// Wait for previous frame
			auto& current_frame_state = RenderSystemContext::GetCurrentFrameState();
			current_frame_state.m_fence_in_flight->Wait();
			try
			{
				wait_for_next_image_index(current_frame_state);
				current_frame_state.m_fence_in_flight->Reset();

				auto& current_commandbuffer = current_frame_state.m_command_buffer;

				static UniformBuffer UBO;
				static time::StopWatch timer{};

				UBO.matrix_model =	glm::rotate(glm::mat4x4(1.0f),
														0.1f * (float)ONE_DEGREE * static_cast<float>(timer.split().milliseconds()),
														glm::vec3(0.0f, 0.0f, 1.0f));

					//make_rotation_matrix(WORLD_AXIS_Z,  * timer.split().milliseconds()).setIdentity();
				UBO.matrix_view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
																		glm::vec3(0.0f, 0.0f, 0.0f),
																		glm::vec3(0.0f, 0.0f, 1.0f));
					//m_camera.GetViewMatrix();//m_camera.GetViewingMatrix();
				UBO.matrix_projection = glm::perspective(glm::radians(45.0f),
															(float)m_vulkan_context->m_swapchain_current_extent.width /
															(float)m_vulkan_context->m_swapchain_current_extent.height,
															0.1f,
															10.0f);
				UBO.matrix_projection[1][1] *= -1.0f;
					//m_camera.GetProjectionMatrix();

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
					m_render_passes[render_pass_forward]->Begin(current_commandbuffer);
					{
						auto& pipelines = m_render_passes[render_pass_forward]->GetGraphicsPipelines();

						pipelines[ForwardRenderPass::pipeline_present]->Bind(current_commandbuffer);
						for (auto& model : m_models)
						{
							model.Draw(*current_commandbuffer);
						}
					}
					m_render_passes[render_pass_forward]->End(current_commandbuffer);
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

	private:
		void wait_for_next_image_index(FrameState& current_frame_state);

		void create_render_passes();
		void handle_window_resize();

		std::vector<std::shared_ptr<ImageFuture>> begin_loading_images();
		void end_loading_images(std::vector<std::shared_ptr<ImageFuture>> image_tasks);

		void load_models();
	};

}} // namespace Albedo::Runtime
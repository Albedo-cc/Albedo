#include "render_system.h"

#include <AlbedoTime.hpp>

#include "camera/camera.h"
#include "paint_box/easel.h"

#include "render_pass/forward_rendering/forward_render_pass.h"
#include "render_pass/UI_rendering/UI_render_pass.h"

#include <core/math/math.h>
#include <runtime/asset_layer/asset_manager.h>
#include <runtime/function_layer/UI/UI_system.h>

namespace Albedo {
namespace Runtime
{
	void RenderSystem::Update()
	{
		try
		{
			auto& canvas = m_easel->WaitCanvas(); // Wait for Next Canvas
			auto& palette = canvas.palette;

			static time::StopWatch timer{};

			// Update Camera
			{
				auto& camera_data = m_camera->Camera_Matrics;
			/*	camera_data.matrix_model = glm::rotate(glm::mat4x4(1.0f),
					0.1f * (float)ONE_DEGREE * static_cast<float>(timer.split().milliseconds()),
					glm::vec3(0.0f, 0.0f, 1.0f));*/

				//make_rotation_matrix(WORLD_AXIS_Z,  * timer.split().milliseconds()).setIdentity();
				camera_data.matrix_view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
					glm::vec3(0.0f, -0.1f, -1.0f),
					glm::vec3(0.0f, 0.0f, 1.0f));
				//m_camera.GetViewMatrix();//m_camera.GetViewingMatrix();
				camera_data.matrix_projection = glm::perspective(glm::radians(80.0f),
					(float)m_vulkan_context->m_swapchain_current_extent.width /
					(float)m_vulkan_context->m_swapchain_current_extent.height,
					0.1f,
					10.0f);
				camera_data.matrix_projection[1][1] *= -1.0f; // Y-Flip

				auto& light_data = m_camera->Light_Parameters;
				light_data.light_position = glm::vec4(5.0f, 5.0f, -5.0f, 1.0f);
				light_data.view_position = glm::vec4(0.0f, -0.1f, -1.0f, 0.0f);
			}
			palette.SetupCameraMatrics(m_camera->GetCameraMatrics());
			palette.SetupLightParameters(m_camera->m_light_parameter_buffer);

			// Render Scene
			canvas.cmd_buffer_front->Begin();
			m_render_passes[render_pass_forward]->Begin(canvas.cmd_buffer_front);
			{
				auto& pipelines = m_render_passes[render_pass_forward]->GetGraphicsPipelines();
				canvas.Paint(canvas.cmd_buffer_front, pipelines[ForwardRenderPass::pipeline_present], m_scene);
			}
			m_render_passes[render_pass_forward]->End(canvas.cmd_buffer_front);
			canvas.cmd_buffer_front->End();

			// Render UI
			if (!wp_system_UI.expired()) // Future:: One-time Command Buffer
			{
				canvas.cmd_buffer_ui->Begin();
				m_render_passes[render_pass_UI]->Begin(canvas.cmd_buffer_ui);
				wp_system_UI.lock()->Render(canvas.cmd_buffer_ui);
				m_render_passes[render_pass_UI]->End(canvas.cmd_buffer_ui);
				canvas.cmd_buffer_ui->End();
			}

			// Submit Command
			std::vector<VkCommandBuffer> commandBuffers{ *canvas.cmd_buffer_front, *canvas.cmd_buffer_ui };
			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			std::vector<VkSemaphore> wait_semaphores{ *canvas.syncmeta.semaphore_image_available };
			std::vector<VkSemaphore> signal_semaphores{ *canvas.syncmeta.semaphore_render_finished };
			VkSubmitInfo submitInfo
			{
				.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.waitSemaphoreCount = static_cast<uint32_t>(wait_semaphores.size()),
				.pWaitSemaphores = wait_semaphores.data(),
				.pWaitDstStageMask = &wait_stage,
				.commandBufferCount = static_cast<uint32_t>(commandBuffers.size()),
				.pCommandBuffers = commandBuffers.data(),
				.signalSemaphoreCount = static_cast<uint32_t>(signal_semaphores.size()),
				.pSignalSemaphores = signal_semaphores.data()
			};

			auto submit_queue = m_vulkan_context->GetQueue(m_vulkan_context->m_device_queue_family_graphics);
			if (vkQueueSubmit(submit_queue, 1, &submitInfo, *canvas.syncmeta.fence_in_flight) != VK_SUCCESS)
				throw std::runtime_error("Failed to submit the Vulkan Command Buffer!");

			m_easel->PresentCanvas({ *canvas.syncmeta.semaphore_render_finished });
			//std::this_thread::sleep_for(std::chrono::seconds(1)); log::debug("Wait 1 s \n\n");
		}
		catch (RHI::VulkanContext::swapchain_error& swapchian_recreation)
		{
			handle_window_resize();
		}
	}

	RenderSystem::RenderSystem(std::shared_ptr<RHI::VulkanContext> vulkan_context) :
		m_vulkan_context{ std::move(vulkan_context)},
		m_camera{ std::make_shared<Camera>(m_vulkan_context) },
		m_easel{ std::make_shared<Easel>(m_vulkan_context) },
		m_scene{ std::make_shared<Scene>(m_vulkan_context) }
	{
		//auto scene_future = AssetManager::instance().AsyncLoadModel("FlightHelmet/FlightHelmet.gltf");
		auto scene_future = AssetManager::instance().AsyncLoadModel("Cube/Cube.gltf");

		// Render Passes
		create_render_passes();

		m_scene->Sketch(scene_future->WaitResult());
	}

	void RenderSystem::create_render_passes()
	{
		m_render_passes.clear();
		m_render_passes.resize(MAX_RENDER_PASS_COUNT);
		m_render_passes[render_pass_forward] = std::make_shared<ForwardRenderPass>(m_vulkan_context);
		m_render_passes[render_pass_UI] = std::make_shared<UIRenderPass>(m_vulkan_context);
	}

	void RenderSystem::ConnectUISystem(std::shared_ptr<UISystem> UI)
	{
		UI->Initialize(m_render_passes[render_pass_UI], UIRenderPass::subpass_UI);
		wp_system_UI = UI;
	}

	void RenderSystem::handle_window_resize()
	{
		log::info("Window Resized");

		m_vulkan_context->RecreateSwapChain();
		create_render_passes();		 // Recreate Render Passes
		m_camera->GetViewingMatrix(true); // Update
	}

}} // namespace Albedo::Runtime
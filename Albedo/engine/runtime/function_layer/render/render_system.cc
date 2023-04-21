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
			static time::StopWatch timer{};

			// Update Camera
			{
				auto& camera_data = m_camera->Camera_Matrics;
			/*	camera_data.matrix_model = glm::rotate(glm::mat4x4(1.0f),
					0.1f * (float)ONE_DEGREE * static_cast<float>(timer.split().milliseconds()),
					glm::vec3(0.0f, 0.0f, 1.0f));*/

				//make_rotation_matrix(WORLD_AXIS_Z,  * timer.split().milliseconds()).setIdentity();
				//static auto eye = glm::vec3(2.0f, 2.0f, 2.0f);  // Cube
				static auto eye = glm::vec3(0.06f, 0.13f, 0.3f); // bottle
				auto& light_data = m_camera->Light_Parameters;
				static bool registered = false;
				if (!registered)
				{
					light_data.light_position = Vector4f(0.1, 0.1, 0.1, 1.0);
					light_data.view_position = Vector4f(0.0f, -0.1f, -0.1f, 0.0f);

					UISystem::instance().RegisterUIEvent(
						"Camera", [&]()
						{
							ImGui::Begin("UBP");

							ImGui::InputFloat("Eye_X", &eye.x);
							ImGui::InputFloat("Eye_Y", &eye.y);
							ImGui::InputFloat("Eye_Z", &eye.z);
							ImGui::Separator();
							ImGui::InputFloat("Light_X", &light_data.light_position[0]);
							ImGui::InputFloat("Light_Y", &light_data.light_position[1]);
							ImGui::InputFloat("Light_Z", &light_data.light_position[2]);

							ImGui::End();
						}
					);
					registered = true;
				}
				camera_data.matrix_view = glm::lookAt(eye,
																						glm::vec3(0.0f, -0.1f, -1.0f),
																						glm::vec3(0.0f, 0.0f, 1.0f));
				//m_camera.GetViewMatrix();//m_camera.GetViewingMatrix();
				camera_data.matrix_projection = glm::perspective(glm::radians(80.0f),
					(float)m_vulkan_context->m_swapchain_current_extent.width /
					(float)m_vulkan_context->m_swapchain_current_extent.height,
					0.1f,
					10.0f);
				camera_data.matrix_projection[1][1] *= -1.0f; // Y-Flip
			}
			Palette::SetupCameraMatrics(m_camera->descriptor_set_ubo, m_camera->GetCameraMatrics());
			m_camera->m_light_parameter_buffer->Write(&m_camera->Light_Parameters);
			Palette::SetupLightParameters(m_camera->descriptor_set_ubo, m_camera->m_light_parameter_buffer);

			// Render Scene
			canvas.cmd_buffer_front->Begin();
			m_render_passes[render_pass_forward]->Begin(canvas.cmd_buffer_front);
			{
				auto& pipelines = m_render_passes[render_pass_forward]->GetGraphicsPipelines();
				Palette::BindDescriptorSetUBO(canvas.cmd_buffer_front,
					pipelines[ForwardRenderPass::pipeline_present],
					m_camera->descriptor_set_ubo);
				canvas.Paint(canvas.cmd_buffer_front, pipelines[ForwardRenderPass::pipeline_present], m_scene);
			}
	
			m_render_passes[render_pass_forward]->End(canvas.cmd_buffer_front);
			canvas.cmd_buffer_front->End();
			canvas.cmd_buffer_front->Submit(false, nullptr, 
				{ *canvas.syncmeta.semaphore_image_available }, 
				{ *canvas.syncmeta.semaphore_render_finished },
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

			// Render UI
			auto& UI = UISystem::instance();
			if (UI.ShouldRender()) // Future:: Transfer Queue Family
			{
				// Render Main Scene to UI System
				auto& swapchain_extent = m_vulkan_context->m_swapchain_current_extent;

				m_vulkan_context->Screenshot(UI.main_scene_image,
					{ *canvas.syncmeta.semaphore_render_finished },
					{ *canvas.syncmeta.semaphore_screenshot_finished });

				//UI.main_scene_image->Write(screenshot);
				UI.main_scene->Update(UI.main_scene_image);

				canvas.cmd_buffer_ui->Begin();
				m_render_passes[render_pass_UI]->Begin(canvas.cmd_buffer_ui);
				UI.Render(canvas.cmd_buffer_ui);
				m_render_passes[render_pass_UI]->End(canvas.cmd_buffer_ui);
				canvas.cmd_buffer_ui->End();
				canvas.cmd_buffer_ui->Submit(false, 
					*canvas.syncmeta.fence_in_flight,
					{ *canvas.syncmeta.semaphore_screenshot_finished },
					{ *canvas.syncmeta.semaphore_ui_finished },
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
			}

			m_easel->PresentCanvas({ *canvas.syncmeta.semaphore_ui_finished });

			//std::this_thread::sleep_for(std::chrono::seconds(1)); log::debug("Wait 1 s \n\n");
		}
		catch (RHI::VulkanContext::swapchain_error& swapchian_recreation)
		{
			handle_window_resize();
		}
	}

	RenderSystem::RenderSystem(std::shared_ptr<RHI::VulkanContext> vulkan_context) :
		m_vulkan_context{ std::move(vulkan_context)},
		m_easel{ std::make_shared<Easel>(m_vulkan_context) },
		m_camera{ std::make_shared<Camera>(m_vulkan_context) },
		m_scene{ std::make_shared<Scene>(m_vulkan_context) }
	{
		//auto scene_future = AssetManager::instance().AsyncLoadModel("FlightHelmet/glTF/FlightHelmet.gltf");
		//auto scene_future = AssetManager::instance().AsyncLoadModel("ABeautifulGame/glTF/ABeautifulGame.gltf");
		//auto scene_future = AssetManager::instance().AsyncLoadModel("Cube/Cube.gltf");
		auto scene_future = AssetManager::instance().AsyncLoadModel("WaterBottle/glTF/WaterBottle.gltf");

		// Render Passes
		create_render_passes();

		// UI System
		UISystem::instance().Initialize(m_vulkan_context, m_render_passes[render_pass_UI], UIRenderPass::subpass_UI);

		m_scene->Sketch(scene_future->WaitResult());
	}

	void RenderSystem::create_render_passes()
	{
		m_render_passes.clear();
		m_render_passes.resize(MAX_RENDER_PASS_COUNT);
		m_render_passes[render_pass_forward] = std::make_shared<ForwardRenderPass>(m_vulkan_context);
		m_render_passes[render_pass_UI] = std::make_shared<UIRenderPass>(m_vulkan_context);
	}


	void RenderSystem::handle_window_resize()
	{
		static bool RECREATING = false;
		if (RECREATING) return;
		RECREATING = true;
		{
			log::info("Window Resized");

			m_vulkan_context->RecreateSwapChain();
			create_render_passes();		 // Recreate Render Passes
			m_camera->GetViewingMatrix(true); // Update
		}
		RECREATING = false;
	}

}} // namespace Albedo::Runtime
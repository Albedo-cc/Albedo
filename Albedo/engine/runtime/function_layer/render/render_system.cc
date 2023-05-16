#include "render_system.h"

#include <imcurio.h>
#include <AlbedoTime.hpp>

#include "camera/camera.h"
#include "paintbox/easel.h"
#include "environment/light.h"
#include "environment/scene.h"

#include "renderpass/forward_rendering/forward_render_pass.h"
#include "renderpass/UI_rendering/UI_render_pass.h"

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

			// Update UBO
			Palette::SetupCameraMatrics(m_easel->GetDescriptorSetUBO(), m_camera->GetCameraMatricsBuffer());
			static Light light{ m_vulkan_context };
			Palette::SetupLightParameters(m_easel->GetDescriptorSetUBO(), light.GetLightData());

			// Render Scene
			canvas.cmd_buffer_front->Begin();
			m_render_passes[render_pass_forward]->Begin(canvas.cmd_buffer_front);
			{
				auto& pipelines = m_render_passes[render_pass_forward]->GetGraphicsPipelines();
				Palette::BindDescriptorSetUBO(canvas.cmd_buffer_front,
					pipelines[ForwardRenderPass::pipeline_present],
					m_easel->GetDescriptorSetUBO());
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
		auto scene_future = AssetManager::instance().AsyncLoadModel("Cube/Cube.gltf");
		//auto scene_future = AssetManager::instance().AsyncLoadModel("WaterBottle/glTF/WaterBottle.gltf");

		m_camera->SetPosition({ 0, 0, -5 });

		// Render Passes
		create_render_passes();

		// UI System
		UISystem::instance().Initialize(m_vulkan_context, m_render_passes[render_pass_UI], UIRenderPass::subpass_UI);

		m_scene->Load(scene_future->WaitResult());

		// Scene Window
		m_chest = std::make_unique<ImCurio::Chest>(
			ImVec2{ 64, 64 },
			24,
			ImCurioChestFlags_AutoIncreaseCapacity);

		//m_chest->InsertItem(m_scene.get());

		UISystem::instance().RegisterUIEvent(
			"Toolkit", [this]()
			{
				ImGui::Begin("Toolkit", 0, ImGuiWindowFlags_NoScrollbar);
				{
					(*m_chest)();
				}
				ImGui::End();
			});
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
			m_vulkan_context->RecreateSwapChain();
			create_render_passes();		 // Recreate Render Passes
		}
		RECREATING = false;
	}

}} // namespace Albedo::Runtime
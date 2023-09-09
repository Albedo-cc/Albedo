#include "UI_system.h"

#include <AlbedoCore/Log/log.h>
#include <AlbedoGraphics/Internal/RHI.h>
#include <AlbedoSystem/Window/window_system.h>

#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

namespace Albedo
{
	void
	UISystem::
	Initialize(const CreateInfo& createinfo)
	{
		assert(g_rhi != nullptr && "Forget to Initialize RHI?");
		// Initialize Dear ImGUI
		ImGui_ImplVulkan_InitInfo ImGui_InitInfo
		{
			.Instance		= g_rhi->instance,
			.PhysicalDevice = g_rhi->GPU,
			.Device			= g_rhi->device,
			.QueueFamily	= g_rhi->device.queue_families.graphics,
			.Queue			= g_rhi->device.queue_families.graphics.queues.front(),
			.PipelineCache	= g_rhi->pipeline_cache,
			.DescriptorPool = createinfo.descriptor_pool,
			.Subpass		= createinfo.subpass,
			.MinImageCount	= g_rhi->swapchain.images.size(),
			.ImageCount		= g_rhi->swapchain.images.size(),
			.MSAASamples	= VK_SAMPLE_COUNT_1_BIT,
			.Allocator		= g_rhi->allocator,
		};

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		auto& io = ImGui::GetIO();

		// Configuration
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable	|
						  ImGuiConfigFlags_ViewportsEnable	;

		/*const char* font_path = "resource/font/calibri.ttf";
		if (io.Fonts->AddFontFromFileTTF(font_path, 16.0f) == NULL)
			Log::Fatal(std::format("Failed to load font {}!", font_path));*/

		ImGui_ImplVulkan_Init(&ImGui_InitInfo, createinfo.renderpass);
		bool INSTALL_IMGUI_CALLBACKS = true;
		ImGui_ImplGlfw_InitForVulkan(WindowSystem::GetWindow(), INSTALL_IMGUI_CALLBACKS); // Install callbacks via ImGUI

		/*auto commandBuffer = m_vulkan_context->
			CreateOneTimeCommandBuffer(m_vulkan_context->m_device_queue_family_graphics);
		commandBuffer->Begin();
		main_scene_image->TransitionLayoutCommand(commandBuffer, 
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		ImGui_ImplVulkan_CreateFontsTexture(*commandBuffer);
		commandBuffer->End();
		commandBuffer->Submit(true);*/

		//ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	void
	UISystem::
	Terminate() noexcept
	{
		m_ui_event_manager.Clear("UI System");

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void
	UISystem::
	Process(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
	{
		assert(commandbuffer->IsRecording());

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			m_ui_event_manager.TrigAll();
		}
		ImGui::Render(); // Prepare the data for rendering so you can call GetDrawData()
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *commandbuffer);
	}

	void
	UISystem::
	RegisterUIEvent(UIEvent* event)
	{
		m_ui_event_manager.Register(event);
	}

	void
	UISystem::
	DeleteUIEvent(std::string_view name)
	{
		m_ui_event_manager.Delete(name);
	}

	bool
	UIEvent::
	Trig()
	{
		return false;
	}

	void
	UIEvent::
	Act()
	{

	}

} // namespace Albedo
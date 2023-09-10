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
		Log::Debug("Albedo UI System is being initialized...");

		assert(g_rhi != nullptr && "Forget to Initialize RHI?");
		assert(!createinfo.font_path.empty());

		// Initialize Dear ImGUI
		ImGui_ImplVulkan_InitInfo ImGui_InitInfo
		{
			.Instance		= g_rhi->instance,
			.PhysicalDevice = g_rhi->GPU,
			.Device			= g_rhi->device,
			.QueueFamily	= g_rhi->device.queue_families.graphics,
			.Queue			= g_rhi->device.queue_families.graphics.queues[0],
			.PipelineCache	= g_rhi->pipeline_cache,
			.DescriptorPool = createinfo.descriptor_pool,
			.Subpass		= createinfo.subpass,
			.MinImageCount	= static_cast<uint32_t>(g_rhi->swapchain.images.size()),
			.ImageCount		= static_cast<uint32_t>(g_rhi->swapchain.images.size()),
			.MSAASamples	= VK_SAMPLE_COUNT_1_BIT,
			.Allocator		= g_rhi->allocator,
		};

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		auto& io = ImGui::GetIO();

		// Configuration
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable
					  ;//|ImGuiConfigFlags_ViewportsEnable;

		if (io.Fonts->AddFontFromFileTTF(createinfo.font_path.data(), createinfo.font_size) == NULL)
			Log::Fatal("Albedo System UI: Failed to load font {}!", createinfo.font_path);

		bool INSTALL_IMGUI_CALLBACKS = true;
		ImGui_ImplGlfw_InitForVulkan(WindowSystem::GetWindow(), INSTALL_IMGUI_CALLBACKS); // Install callbacks via ImGUI
		ImGui_ImplVulkan_Init(&ImGui_InitInfo, createinfo.renderpass);

		auto commandbuffer =
			GRI::GetGlobalCommandPool(CommandPoolType_Transient, QueueFamilyType_Graphics)
			->AllocateCommandBuffer({ .level = CommandBufferLevel_Primary });
		
		commandbuffer->Begin();
		{
			ImGui_ImplVulkan_CreateFontsTexture(*commandbuffer);
		}
		commandbuffer->End();
		commandbuffer->Submit({.wait_stages = VK_PIPELINE_STAGE_TRANSFER_BIT});

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	void
	UISystem::
	Terminate() noexcept
	{
		Log::Debug("Albedo UI System is being terminated...");

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
		ImGui::UpdatePlatformWindows();
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

} // namespace Albedo
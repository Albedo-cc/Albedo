#include "editor.h"

#include <Albedo/Core/Log/log.h>
#include <Albedo/Core/Norm/assert.h>
#include <Albedo/Core/Norm/assert.h>
#include <Albedo/Graphics/RHI.h>
#include <Albedo/Graphics/Internal/Vulkan.h>
#include <Albedo/System/Window/window_system.h>
#include <Albedo/Platform/path.h>

#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

#include "Internal/editorpass.h"

namespace Albedo
{
	void
	Editor::
	Initialize(CreateInfo createinfo)
	{
		Log::Debug("Albedo Editor is being initialized...");
		m_settings = std::move(createinfo);

		sm_renderpass = std::make_shared<EditorPass>();

		ALBEDO_ASSERT(g_vk != nullptr && "Forget to Initialize RHI?");

		// Initialize Dear ImGUI
		ImGui_ImplVulkan_InitInfo ImGui_InitInfo
		{
			.Instance		= g_vk->instance,
			.PhysicalDevice = g_vk->GPU,
			.Device			= g_vk->device,
			.QueueFamily	= g_vk->device.queue_families.graphics,
			.Queue			= g_vk->device.queue_families.graphics.queues[0],
			.PipelineCache	= g_vk->pipeline_cache,
			.DescriptorPool = *RHI::GetGlobalDescriptorPool(),
			.Subpass		= EditorPass::Subpass::ImGui,
			.MinImageCount	= static_cast<uint32_t>(g_vk->swapchain.images.size()),
			.ImageCount		= static_cast<uint32_t>(g_vk->swapchain.images.size()),
			.MSAASamples	= VK_SAMPLE_COUNT_1_BIT,
			.Allocator		= g_vk->allocator,
		};

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		auto& io = ImGui::GetIO();

		// Configuration
		io.IniFilename = m_settings.layout.c_str();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable
					  ;//|ImGuiConfigFlags_ViewportsEnable;

		if (io.Fonts->AddFontFromFileTTF(m_settings.font.c_str(), createinfo.font_size) == NULL)
			Log::Fatal("Albedo System UI: Failed to load font {}!", m_settings.font.c_str());

		bool INSTALL_IMGUI_CALLBACKS = true;
		ImGui_ImplGlfw_InitForVulkan(WindowSystem::GetWindow(), INSTALL_IMGUI_CALLBACKS); // Install callbacks via ImGUI
		ImGui_ImplVulkan_Init(&ImGui_InitInfo, *sm_renderpass);

		sm_frame_infos.resize(RHI::GetRenderTargetCount());

		Fence fence{ FenceType_Unsignaled };
		auto commandbuffer =
			RHI::GetGlobalCommandPool(
				CommandPoolType_Transient,
				QueueFamilyType_Graphics)
			->AllocateCommandBuffer({ .level = CommandBufferLevel_Primary });

		std::vector<VkWriteDescriptorSet> destwriteinfo;

		commandbuffer->Begin();
		{
			ImGui_ImplVulkan_CreateFontsTexture(*commandbuffer);
			for (auto& frame_info : sm_frame_infos)
			{
				frame_info.descriptor_set = RHI::GetGlobalDescriptorPool()
				->AllocateDescriptorSet(RHI::GetGlobalDescriptorSetLayout
				(RHI::MakeID({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER})));

				frame_info.main_camera =
					Texture2D::Create(Texture::CreateInfo
					{
					.aspect = VK_IMAGE_ASPECT_COLOR_BIT,
					.usage  = VK_IMAGE_USAGE_SAMPLED_BIT |
							  VK_IMAGE_USAGE_TRANSFER_DST_BIT,
					.format = RHI::GetRenderTargetFormat(),
					.extent = {uint32_t(sm_main_camera_extent.x),
							   uint32_t(sm_main_camera_extent.y), 1},
					.mipLevels	 = 1,
					.arrayLayers = 1,
					.samples = VK_SAMPLE_COUNT_1_BIT,
					.tiling  = VK_IMAGE_TILING_OPTIMAL,
					});

				frame_info.main_camera->ConvertLayout(commandbuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

				// Update Descriptor Sets
				destwriteinfo.emplace_back(
					frame_info.descriptor_set
					->BindToTexture(0, frame_info.main_camera));

				frame_info.commandbuffer =
					RHI::GetGlobalCommandPool(
						CommandPoolType_Resettable,
						QueueFamilyType_Graphics)
					->AllocateCommandBuffer({ .level = CommandBufferLevel_Primary });
			}
		}
		commandbuffer->End();
		commandbuffer->Submit({}, fence);
		fence.Wait();

		DescriptorSet::UpdateInBatch(destwriteinfo);

		ImGui_ImplVulkan_DestroyFontUploadObjects();

		// Register basic UI
		RegisterUIEvent(new UIEvent{
			"Editor::MainCamera", []()
			{
				auto& frame = sm_frame_infos[RHI::GetRenderTargetCursor()];

				ImGui::SetNextWindowSize(sm_main_camera_extent);
				ImGui::Begin("Main Scene", nullptr,
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoScrollbar |
					ImGuiWindowFlags_NoScrollWithMouse);
				{
					ImGui::Image(*frame.descriptor_set, {1080,720});
					sm_main_camera_extent = ImGui::GetWindowSize();
				}
				ImGui::End();
				ImGui::ShowDemoWindow();
			}});
		
		// Enable
		m_enabled = true;
	}

	void
	Editor::
	Terminate() noexcept
	{
		Log::Debug("Albedo Editor is being terminated...");

		sm_ui_event_manager.Clear("Editor");
		sm_frame_infos.clear();
		sm_renderpass.reset();

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	Editor::FrameInfo&
	Editor::Render()
	{
		auto& frame = sm_frame_infos[RHI::GetRenderTargetCursor()];

		//if (frame.main_camera->GetExtent().width  != sm_main_camera_extent.x ||
		//	frame.main_camera->GetExtent().height != sm_main_camera_extent.y)
		//{
		//	auto cmdbuffer = 
		//		RHI::GetGlobalCommandPool(
		//		CommandPoolType_Transient,
		//		QueueFamilyType_Graphics)
		//		->AllocateCommandBuffer({
		//		.level = CommandBufferLevel_Primary});
		//	frame.main_camera =
		//		Texture2D::Create(Texture::CreateInfo
		//		{
		//		.aspect = VK_IMAGE_ASPECT_COLOR_BIT,
		//		.usage  = VK_IMAGE_USAGE_SAMPLED_BIT |
		//					VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		//		.format = RHI::GetRenderTargetFormat(),
		//		.extent = {uint32_t(sm_main_camera_extent.x),
		//					uint32_t(sm_main_camera_extent.y), 1},
		//		.mipLevels	 = 1,
		//		.arrayLayers = 1,
		//		.samples = VK_SAMPLE_COUNT_1_BIT,
		//		.tiling  = VK_IMAGE_TILING_OPTIMAL,
		//		});

		//	cmdbuffer->Begin();
		//	{
		//		//frame.main_camera->Resize(cmdbuffer, { uint32_t(window_extent.x), uint32_t(window_extent.y) });
		//		frame.main_camera->ConvertLayout(cmdbuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		//	}
		//	cmdbuffer->End();
		//	Fence fence{ FenceType_Unsignaled };
		//	cmdbuffer->Submit({}, fence);
		//	fence.Wait();
		//}

		frame.commandbuffer->Begin();
		{
			// Capture current scene
			RHI::GetCurrentRenderTarget()->Blit(frame.commandbuffer, frame.main_camera);

			auto subpass_iter = sm_renderpass->Begin(frame.commandbuffer);
			{
				ALBEDO_ASSERT_DEBUG(subpass_iter.GetName() == "Editor::ImGui");
				subpass_iter.Begin();
				{
					sm_ui_event_manager.Process();
				}
				subpass_iter.End();
				ALBEDO_ASSERT(!subpass_iter.Next());
			}
			sm_renderpass->End(frame.commandbuffer);
		}
		frame.commandbuffer->End();
		return frame;
	}

	void
	Editor::Recreate()
	{
		sm_renderpass = std::make_shared<EditorPass>();

		for (auto& frame_info : sm_frame_infos)
		{
			frame_info.commandbuffer =
				RHI::GetGlobalCommandPool(
					CommandPoolType_Resettable,
					QueueFamilyType_Graphics)
				->AllocateCommandBuffer({ .level = CommandBufferLevel_Primary });
		}
	}

	void Editor::Enable(const char* signature)
	{
		Log::Debug("{} enabled Albedo Editor.", signature);
		m_enabled = false;
	}

	void Editor::Disable(const char* signature)
	{
		Log::Debug("{} disabled Albedo Editor.", signature);
		m_enabled = true;
	}

	void
	Editor::
	RegisterUIEvent(UIEvent* event)
	{
		sm_ui_event_manager.Register(event);
	}

	void
	Editor::
	DeleteUIEvent(std::string_view name)
	{
		sm_ui_event_manager.Delete(name);
	}

} // namespace Albedo
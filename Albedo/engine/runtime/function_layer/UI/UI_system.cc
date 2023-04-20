#include "UI_system.h"

#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#include "UI_window/UI_window.h";

namespace Albedo {
namespace Runtime
{

	UISystem::~UISystem()
	{
		if (m_should_render.has_value())
		{
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
		else log::critical("System did not been initialized!");
	}

	void UISystem::Initialize(std::shared_ptr<RHI::VulkanContext> vulkan_context, std::shared_ptr<RHI::RenderPass> render_pass, uint32_t subpass)
	{
		assert(m_vulkan_context == nullptr && "You cannot reinitialize UI System!");
		m_vulkan_context = std::move(vulkan_context);

		auto& graphics_queue_family = m_vulkan_context->m_device_queue_family_graphics;
		ImGui_ImplVulkan_InitInfo ImGui_InitInfo
		{
			.Instance = m_vulkan_context->m_instance,
			.PhysicalDevice = m_vulkan_context->m_physical_device,
			.Device = m_vulkan_context->m_device,
			.QueueFamily = graphics_queue_family.value(),
			.Queue = m_vulkan_context->GetQueue(graphics_queue_family),
			.PipelineCache = VK_NULL_HANDLE,
			.DescriptorPool = *m_vulkan_context->GetGlobalDescriptorPool(),
			.Subpass = subpass,
			.MinImageCount = m_vulkan_context->m_swapchain_image_count,
			.ImageCount = m_vulkan_context->m_swapchain_image_count,
			.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
			.Allocator = m_vulkan_context->m_memory_allocation_callback
		};

		// Create Main Scene
		float width = 1000, height = 600;
		auto sampler = m_vulkan_context->CreateSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT);
		auto& swapchain_extent = m_vulkan_context->m_swapchain_current_extent;
		main_scene_image = m_vulkan_context->m_memory_allocator->
			AllocateImage(VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT,
				width, height, 4,
				//swapchain_extent.width, swapchain_extent.height, 4,
				VK_FORMAT_R8G8B8A8_SRGB);
		main_scene_image->BindSampler(sampler);
		main_scene = CreateWidgetTexture(main_scene_image);
		RegisterUIEvent(
			"Main Scene",
			[this]()->void 
			{
				float width = 1000, height = 600;
				ImGui::SetNextWindowSize({ width, height });
				//ImGui::SetNextWindowPos({200, 200 });
				ImGui::Begin("Main Scene", nullptr, 
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoScrollbar |
					ImGuiWindowFlags_NoScrollWithMouse);
				ImGui::Image(*main_scene, { width, height });
				auto pos = ImGui::GetWindowPos();
				ImGui::SetCursorPos(pos);
				ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

				ImGui::End();
			});

		// Initialize Dear ImGUI
		ImGui::CreateContext();
		auto& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui_ImplVulkan_Init(&ImGui_InitInfo, *render_pass);
		ImGui_ImplGlfw_InitForVulkan(m_vulkan_context->m_window, true); // Install callbacks via ImGUI

		auto commandBuffer = m_vulkan_context->
			CreateOneTimeCommandBuffer(m_vulkan_context->m_device_queue_family_graphics);
		commandBuffer->Begin();
		main_scene_image->TransitionLayoutCommand(commandBuffer, 
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		ImGui_ImplVulkan_CreateFontsTexture(*commandBuffer);
		commandBuffer->End();
		commandBuffer->Submit(true);

		ImGui_ImplVulkan_DestroyFontUploadObjects();

		m_should_render = true;
	}

	void UISystem::RegisterUIEvent(std::string name, UIEvent event)
	{
		auto target = m_ui_events.find(name);
		if (target == m_ui_events.end())
		{
			log::info("Albedo UI System: Registered a new UI event {}", name);
			m_ui_events[name] = std::move(event);
		}
		else log::warn("Albedo UI System: Failed to register UI event {}", name);
	}

	std::shared_ptr<UIWidget::Texture> UISystem::
		CreateWidgetTexture(std::shared_ptr<RHI::VMA::Image> image)
	{
		static auto imgui_texture_set_layout = m_vulkan_context->
			CreateDescripotrSetLayout({ {
			0,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			1,
			VK_SHADER_STAGE_FRAGMENT_BIT} });

		return std::make_shared<UIWidget::Texture>(m_vulkan_context->CreateDescriptorSet(imgui_texture_set_layout));
	}

	void UISystem::Render(std::shared_ptr<RHI::CommandBuffer> commandBuffer)
	{

		assert(m_vulkan_context != nullptr && "You must call Initialize() before Render()!");
		if (m_should_render)
		{
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			
			{ // TEST
				static UIWindow main_window{};
				main_window.Render();
			}

			ImGui::ShowDemoWindow();
			for (const auto& [name, ui_event] : m_ui_events)
			{
				ui_event();
			}
		
			ImGui::Render(); // Prepare the data for rendering so you can call GetDrawData()
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *commandBuffer);
		}
	}

}} // namespace Albedo::Runtime
#include "UI_system.h"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

namespace Albedo {
namespace Runtime
{

	UISystem::UISystem(std::shared_ptr<RHI::VulkanContext> vulkan_context) :
		m_vulkan_context{ std::move(vulkan_context) } 
	{
		create_descriptor_pool();
	}

	UISystem::~UISystem()
	{
		if (m_should_render.has_value())
		{
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
		else log::critical("UI System did not been initialized!");
	}

	void UISystem::Initialize(std::shared_ptr<RHI::RenderPass> render_pass, uint32_t subpass)
	{
		uint32_t graphics_queue_family = m_vulkan_context->m_device_queue_family_graphics.value();
		ImGui_ImplVulkan_InitInfo ImGui_InitInfo
		{
			.Instance = m_vulkan_context->m_instance,
			.PhysicalDevice = m_vulkan_context->m_physical_device,
			.Device = m_vulkan_context->m_device,
			.QueueFamily = graphics_queue_family,
			.Queue = m_vulkan_context->GetQueue(graphics_queue_family),
			.PipelineCache = VK_NULL_HANDLE,
			.DescriptorPool = *m_descriptor_pool,
			.Subpass = subpass,
			.MinImageCount = m_vulkan_context->m_swapchain_image_count,
			.ImageCount = m_vulkan_context->m_swapchain_image_count,
			.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
			.Allocator = m_vulkan_context->m_memory_allocation_callback
		};

		// Initialize Dear ImGUI
		ImGui::CreateContext();

		ImGui_ImplVulkan_Init(&ImGui_InitInfo, *render_pass);
		ImGui_ImplGlfw_InitForVulkan(m_vulkan_context->m_window, true); // 2nd para: auto install callbacks via ImGUI

		auto commandBuffer = m_vulkan_context->GetOneTimeCommandBuffer();
		commandBuffer->Begin();
		ImGui_ImplVulkan_CreateFontsTexture(*commandBuffer);
		commandBuffer->End();
		commandBuffer->Submit(true);

		ImGui_ImplVulkan_DestroyFontUploadObjects();

		m_should_render = true;
	}

	void UISystem::Render(std::shared_ptr<RHI::CommandBuffer> commandBuffer)
	{
		assert(m_should_render.has_value() && "You must call Initialize() before Render()!");
		if (m_should_render)
		{
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();

			ImGui::ShowDemoWindow();
			ImGui::Render(); // Prepare the data for rendering so you can call GetDrawData()
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *commandBuffer);
		}
	}

	void UISystem::create_descriptor_pool()
	{
		uint32_t oversize = 100;
		m_descriptor_pool = m_vulkan_context->CreateDescriptorPool(
			std::vector<VkDescriptorPoolSize>
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, oversize },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, oversize },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, oversize },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, oversize },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, oversize },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, oversize },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, oversize },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, oversize },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, oversize },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, oversize },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, oversize }
		}, oversize);

	}

}} // namespace Albedo::Runtime
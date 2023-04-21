#include "UI_window.h"

#include <AlbedoRHI.hpp>

namespace Albedo {
namespace Runtime
{

	UIWindow::UIWindow(std::weak_ptr<RHI::VulkanContext> vulkan_context_view):
		m_vulkan_context_view{ std::move(vulkan_context_view) }
	{
		m_window_title = "Docker_Main";
		m_window_flags |=
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;
	}

	void UIWindow::Render()
	{
		ImGuiDockNodeFlags m_docker_flags = ImGuiDockNodeFlags_None;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		Begin();
		{
			ImGui::PopStyleVar(3);
			ImGuiID dockspace_id = ImGui::GetID(m_window_title.c_str());
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), m_docker_flags);


			static bool item_vulkan_context = true;
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Options"))
                {
                    ImGui::MenuItem("Vulkan Context", NULL, &item_vulkan_context);
                    ImGui::Separator();

                    ImGui::EndMenu();
                }

				static bool a, b, c;
				if (ImGui::BeginMenu("Utils"))
				{
					ImGui::MenuItem("Palette", NULL, &a);

				/*	ImGui::Separator();
					ImGui::MenuItem("BBBBBVBB", NULL, &b);
					ImGui::Separator();
					ImGui::MenuItem("CCCCCCCC", NULL, &c);*/

					ImGui::EndMenu();
				}

                ImGui::EndMenuBar();

				if (a)
				{
					// Always both a small version of both types of pickers (to make it more visible in the demo to people who are skimming quickly through it)

					static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
					ImGui::Begin("Palette", nullptr,
						ImGuiWindowFlags_NoTitleBar |
						ImGuiWindowFlags_NoCollapse |
						ImGuiWindowFlags_NoBackground |
						ImGuiWindowFlags_NoScrollbar |
						ImGuiWindowFlags_NoScrollWithMouse);
					

					auto hdr = false;
					ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | ImGuiColorEditFlags_NoDragDrop |  ImGuiColorEditFlags_AlphaPreview |  ImGuiColorEditFlags_NoOptions;
					//ImGui::ColorEdit4("MyColor##3", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | misc_flags);
					ImGui::SameLine();
					ImGui::BeginGroup(); // Lock X position
					ImGui::ColorPicker3("##MyColor##6", (float*)&color,
						ImGuiColorEditFlags_PickerHueWheel |
						ImGuiColorEditFlags_NoSidePreview |
						ImGuiColorEditFlags_NoAlpha);

					ImGui::EndGroup();

					ImGui::End();
				}

				if (item_vulkan_context) menu_item_vulkan_context(&item_vulkan_context);

            } // End Menu Bar

			ImGui::Begin("Look At Me");
			if (ImGui::TreeNode("Configuration##2"))
			{
				bool a = false;
				ImGui::Checkbox("Hello World", &a);				
				ImGui::Checkbox("Hello Worl2d", &a);
				ImGui::TreePop();
				ImGui::Spacing();
			}
			ImGui::End();
		}
		End();
	}

	void UIWindow::menu_item_vulkan_context(bool* is_open)
	{
		if (!m_vulkan_context_view.expired())
		{
			auto vulkan_context = m_vulkan_context_view.lock();
			ImGui::Begin("Vulkan Context", is_open, ImGuiWindowFlags_NoCollapse);

			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
			ImGui::Text("Owners: %u", vulkan_context.use_count());


			if (ImGui::TreeNode("GPU"))
			{
				auto& gpu_properties = vulkan_context->m_physical_device_properties;
				ImGui::Text("GPU: %s", gpu_properties.deviceName);
				ImGui::Text("Max Bound DS: %u", gpu_properties.limits.maxBoundDescriptorSets);
				ImGui::TreePop();
				ImGui::Spacing();
			}

			if (ImGui::TreeNode("Swap Chain"))
			{
				ImGui::Text("Image Width:\t%u", vulkan_context->m_swapchain_current_extent.width);
				ImGui::Text("Image Height:\t%u", vulkan_context->m_swapchain_current_extent.height);
				ImGui::Text("Image Count:\t%u", vulkan_context->m_swapchain_image_count);
				ImGui::TreePop();
				ImGui::Spacing();
			}

			ImGui::End();
		}
	}

}} // namespace Albedo::Runtime
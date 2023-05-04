#include "UI_window.h"

#include <AlbedoRHI.hpp>

#include <runtime/function_layer/control/control_system.h>
#include <net/net.h>

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
			static bool item_user_inputs = true;
			static bool item_console = true;
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("View"))
                {
                    ImGui::MenuItem("Vulkan Context", NULL, &item_vulkan_context);
                    ImGui::Separator();

					ImGui::MenuItem("User Inputs", NULL, &item_user_inputs);
					ImGui::Separator();
					
					ImGui::MenuItem("Console", NULL, &item_console);
					ImGui::Separator();

                    ImGui::EndMenu();
                }

				static bool utils_palette;
				if (ImGui::BeginMenu("Utils"))
				{
					ImGui::MenuItem("Palette", NULL, &utils_palette);

					ImGui::EndMenu();
				}

                ImGui::EndMenuBar();

				if (utils_palette)
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
				if (item_user_inputs) menu_item_input_info(&item_user_inputs);
				if (item_console) menu_item_console(&item_console);

            } // End Menu Bar

		}
		End();
	}

	void UIWindow::menu_item_console(bool* is_open)
	{
		static char buffer_input[128];

		auto& netmodule = Net::NetModule::instance();

		ImGui::Begin("Console", is_open, ImGuiWindowFlags_NoCollapse);
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.2,0.2,0.2,1 });
		
			if (ImGui::BeginListBox("Online Servers", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (const auto& console_message : netmodule.console_messages)
				{
					ImGui::TextUnformatted(console_message.c_str());
				}

				ImGui::SetScrollHereY(1.0f);
				ImGui::EndListBox();
			}
			ImGui::PopStyleColor();

			ImGui::SetNextItemWidth(-FLT_MIN);//ImVec2(-FLT_MIN, 30 * ImGui::GetTextLineHeightWithSpacing())
			if (ImGui::InputText("##INPUT", buffer_input, sizeof(buffer_input) / sizeof(char), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				ImGui::SetKeyboardFocusHere(-1);
				netmodule.console_messages.emplace_back(std::move(std::format("[Me]: {}", buffer_input)));
				netmodule.SyncMessage(buffer_input);
				buffer_input[0] = '\0';
			}
		}
		ImGui::End();
	}

	void UIWindow::menu_item_vulkan_context(bool* is_open)
	{
		if (!m_vulkan_context_view.expired())
		{
			auto vulkan_context = m_vulkan_context_view.lock();
			ImGui::Begin("Vulkan Context", is_open, ImGuiWindowFlags_NoCollapse);
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				if (ImGui::TreeNode("Basic"))
				{
					ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
					ImGui::Text("Owners: %u", vulkan_context.use_count());

					ImGui::TreePop();
					ImGui::Spacing();
				}

				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				if (ImGui::TreeNode("GPU"))
				{
					auto& gpu_properties = vulkan_context->m_physical_device_properties;
					ImGui::Text("GPU: %s", gpu_properties.deviceName);
					ImGui::Text("Max Bound DS: %u", gpu_properties.limits.maxBoundDescriptorSets);

					ImGui::TreePop();
					ImGui::Spacing();
				}

				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				if (ImGui::TreeNode("Swap Chain"))
				{
					ImGui::Text("Image Width:\t%u", vulkan_context->m_swapchain_current_extent.width);
					ImGui::Text("Image Height:\t%u", vulkan_context->m_swapchain_current_extent.height);
					ImGui::Text("Image Count:\t%u", vulkan_context->m_swapchain_image_count);

					ImGui::TreePop();
					ImGui::Spacing();
				}

			}
			ImGui::End();
		}
	}

	void UIWindow::menu_item_input_info(bool* is_open)
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Begin("User Inputs", is_open, ImGuiWindowFlags_NoCollapse);
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Mouse"))
			{
				if (ImGui::IsMousePosValid())
					ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
				else
					ImGui::Text("Mouse pos: <INVALID>");
				ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
				ImGui::Text("Mouse down:");
				for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
					if (ImGui::IsMouseDown(i)) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
				ImGui::Text("Mouse wheel: %.1f", io.MouseWheel);

				ImGui::TreePop();
				ImGui::Spacing();
			}

			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Keyboard"))
			{
				struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key < 512 && ImGui::GetIO().KeyMap[key] != -1; } }; // Hide Native<>ImGuiKey duplicates when both exists in the array
				ImGuiKey start_key = (ImGuiKey)0;

				ImGui::Text("Keys down:");         for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) { if (funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue; ImGui::SameLine(); ImGui::Text((key < ImGuiKey_NamedKey_BEGIN) ? "\"%s\"" : "\"%s\" %d", ImGui::GetKeyName(key), key); }
				ImGui::Text("Keys mods: %s%s%s%s",
					io.KeyCtrl ? "CTRL " : "",	io.KeyShift ? "SHIFT " : "",
					io.KeyAlt ? "ALT " : "",		io.KeySuper ? "SUPER " : "");
				ImGui::Text("Chars queue:");       for (int i = 0; i < io.InputQueueCharacters.Size; i++) { ImWchar c = io.InputQueueCharacters[i]; ImGui::SameLine();  ImGui::Text("\'%c\' (0x%04X)", (c > ' ' && c <= 255) ? (char)c : '?', c); } // FIXME: We should convert 'c' to UTF-8 here but the functions are not public.

				ImGui::TreePop();
				ImGui::Spacing();
			}
		}
		ImGui::End();
	}

}} // namespace Albedo::Runtime
#include "UI_window.h"

namespace Albedo {
namespace Runtime
{

	UIWindow::UIWindow()
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

			static bool a, b, c;
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Options"))
                {
                    ImGui::MenuItem("AAAAAAA", NULL, &a);
                    ImGui::Separator();
					ImGui::MenuItem("BBBBBVBB", NULL, &b);
                    ImGui::Separator();
					ImGui::MenuItem("CCCCCCCC", NULL, &c);

                    ImGui::EndMenu();
                }

				if (ImGui::BeginMenu("Utils"))
				{
					ImGui::MenuItem("Palette", NULL, &a);

					ImGui::Separator();
					ImGui::MenuItem("BBBBBVBB", NULL, &b);
					ImGui::Separator();
					ImGui::MenuItem("CCCCCCCC", NULL, &c);

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
					ImGui::ColorPicker3("##MyColor##6", (float*)&color,
						ImGuiColorEditFlags_PickerHueWheel |
						ImGuiColorEditFlags_NoSidePreview |
						//ImGuiColorEditFlags_NoInputs |
						ImGuiColorEditFlags_NoAlpha);
					//ImGui::SameLine();
					//ImGui::BeginGroup(); // Lock X position
					//ImGui::InputFloat("R", &color.x, 0, 0, "%.0f");
					//ImGui::InputFloat("G", &color.y, 0, 0, "%.0f");
					//ImGui::InputFloat("B", &color.z, 0, 0, "%.0f");
					//ImGui::InputFloat("A", &color.w, 0, 0, "%.0f");
					//ImGui::EndGroup();

					ImGui::End();
				}
            }
		}
		End();
	}

}} // namespace Albedo::Runtime
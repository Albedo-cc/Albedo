#pragma once

#include <imgui.h>
#include <string>

namespace Albedo {
namespace Runtime
{

	class UIWindow
	{
	public:
		virtual void Render();

		UIWindow();

	protected:
		void Begin(){ ImGui::Begin(m_window_title.c_str(), (bool*)m_is_closable, m_window_flags); }
		void End() { ImGui::End(); }

	protected:
		std::string m_window_title;
		void* m_is_closable = NULL;
		ImGuiWindowFlags m_window_flags;
		ImVec2 m_window_size;
		ImVec2 m_window_position;
		
	};

}} // namespace Albedo::Runtime
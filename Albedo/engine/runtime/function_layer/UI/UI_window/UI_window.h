#pragma once

#include <imgui.h>
#include <string>
#include <memory>

namespace Albedo {
	namespace RHI { class VulkanContext; } // Predeclaration
namespace Runtime
{

	class UIWindow
	{
	public:
		virtual void Render();

		UIWindow() = delete;
		UIWindow(std::weak_ptr<RHI::VulkanContext> vulkan_context_view);

	protected:
		void Begin(){ ImGui::Begin(m_window_title.c_str(), (bool*)m_is_closable, m_window_flags); }
		void End() { ImGui::End(); }

	protected:
		std::weak_ptr<RHI::VulkanContext> m_vulkan_context_view;

		std::string m_window_title;
		void* m_is_closable = NULL;
		ImGuiWindowFlags m_window_flags;
		ImVec2 m_window_size;
		ImVec2 m_window_position;
		
	private:
		void menu_item_vulkan_context(bool* is_open);
	};

}} // namespace Albedo::Runtime
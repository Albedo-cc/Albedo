#include "runtime.h"

#include "function_layer/render/render_system.h"
#include "function_layer/window/window_system.h"
#include "function_layer/UI/UI_system.h"

namespace Albedo {
namespace Runtime
{

	RuntimeModule::RuntimeModule() : 
		m_window_system{ std::make_shared<WindowSystem>() }
	{
		m_vulkan_context = RHI::VulkanContext::Create(m_window_system->GetWindow());
		m_render_system = std::make_shared<RenderSystem>(m_vulkan_context);
		m_ui_system = std::make_shared<UISystem>(m_vulkan_context);
		
		WindowSystem::SetFramebufferResizeCallback([this]() { m_render_system->handle_window_resize(); });
		m_render_system->ConnectUISystem(m_ui_system);
	}

	void RuntimeModule::Run()
	{
		while (!m_window_system->ShouldClose())
		{
			m_window_system->Update();
			m_render_system->Update();
		}
	}

}} // namespace Albedo::Runtime
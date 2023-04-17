#include "runtime.h"

#include "function_layer/render/render_system.h"
#include "function_layer/window/window_system.h"
#include "function_layer/UI/UI_system.h"
#include "function_layer/control/control_system.h"

namespace Albedo {
namespace Runtime
{

	RuntimeModule::RuntimeModule() : 
		m_window_system{ std::make_shared<WindowSystem>() }
	{
		m_vulkan_context = RHI::VulkanContext::Create(m_window_system->GetWindow());
		ControlSystem::instance().Initialize(m_window_system->GetWindow()); 
		// Must init Render System after Control System (GLFW callbacks)
		m_render_system = std::make_shared<RenderSystem>(m_vulkan_context); 

		ControlSystem::instance().RegisterKeyboardEvent(
			"Test", KeyboardEvent
			{
				.key = Keyboard::Key::Space,
				.action = Action::Press,
				.function = [] () {log::info("Pressed Space!"); }
			});
		WindowSystem::SetFramebufferResizeCallback([this]() { m_render_system->handle_window_resize(); });
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
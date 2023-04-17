#include "control_system.h"

namespace Albedo {
namespace Runtime
{
	
	void ControlSystem::Initialize(GLFWwindow* window)
	{
		glfwSetKeyCallback(window, keyboard_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);
	}

	void ControlSystem::RegisterKeyboardEvent(std::string name, KeyboardEvent event)
	{
		auto target = m_keyborad_events.find(name);
		if (target == m_keyborad_events.end())
		{
			log::info("Albedo Control System: Registered a new keyboard event {}", name);
			m_keyborad_events[name] = std::move(event);
		}
		else log::warn("Albedo Control System: Failed to register keyboard event {}", name);
	}

	void ControlSystem::RegisterMouseButtonEvent(std::string name, MouseButtonEvent event)
	{
		auto target = m_mouse_button_events.find(name);
		if (target == m_mouse_button_events.end())
		{
			log::info("Albedo Control System: Registered a new mouse event {}", name);
			m_mouse_button_events[name] = std::move(event);
		}
		else log::warn("Albedo Control System: Failed to register mouse event {}", name);
	}

	void ControlSystem::DeleteKeyboardEvent(std::string name)
	{
		auto target = m_keyborad_events.find(name);
		if (target != m_keyborad_events.end())
		{
			log::info("Albedo Control System: Deleted a keyboard event {}", name);
			m_keyborad_events.erase(target);
		}
		else log::warn("Albedo Control System: Failed to delete keyboard event {}", name);
	}

	void ControlSystem::DeleteMouseEvent(std::string name)
	{
		auto target = m_mouse_button_events.find(name);
		if (target != m_mouse_button_events.end())
		{
			log::info("Albedo Control System: Deleted a mouse event {}", name);
			m_mouse_button_events.erase(target);
		}
		else log::warn("Albedo Control System: Failed to delete mouse event {}", name);
	}

	void ControlSystem::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		for (const auto& [name, event] : m_keyborad_events)
		{
			if (event.key == key && event.action == action)
				event.function();
		}
	}

	void ControlSystem::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		for (const auto& [name, event] : m_mouse_button_events)
		{
			if (event.button == button && event.action == action)
				event.function();
		}
	}

}} // namespace Albedo::Runtime